#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

#include "branch.h"
#include "checkout.h"
#include "commands.h"
#include "commit.h"
#include "config.h"
#include "diff.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "merge.h"
#include "object.h"
#include "ref.h"
#include "tag.h"
#include "tree.h"

namespace po = boost::program_options;

void conflicting_options(const po::variables_map& vm,
	const char* opt1, const char* opt2)
{
	if (vm.count(opt1) && !vm[opt1].defaulted()
		&& vm.count(opt2) && !vm[opt2].defaulted())
		throw std::logic_error(std::string("Conflicting options '")
			+ opt1 + "' and '" + opt2 + "'.");
}

void option_dependency(const po::variables_map& vm,
	const char* for_what, const char* required_option)
{
	if (vm.count(for_what) && !vm[for_what].defaulted())
		if (vm.count(required_option) == 0 || vm[required_option].defaulted())
			throw std::logic_error(std::string("Option '") + for_what
				+ "' requires option '" + required_option + "'.");
}

void at_least_one_required(const po::variables_map& vm, const std::vector<std::string>& options)
{
	for (auto option : options)
	{
		if (vm.count(option.c_str()))
			return;
	}
	throw Exception(boost::format("at least one of %1% is required") % boost::join(options, ", "));
}

void init_filesystem()
{
	Filesystem::create_directory(Globals::SimpleGitDir, Filesystem::FILE_FLAG_HIDDEN);
	Filesystem::create_directory(Globals::ObjectDir);
	Filesystem::create_directory(Globals::RefDir);
	Filesystem::create_directory(Globals::TagDir);
	Filesystem::create_directory(Globals::BranchDir);

	Filesystem::write_content(Globals::ConfigFile, "", Filesystem::FILE_FLAG_OVERWRITE);
}

// initializes stuff that are needed for the actual repository commands
void init_for_git_commands()
{
	Globals::Config = read_config(Globals::ConfigFile);
	if (Globals::Config.find("debug") != Globals::Config.end())
		Globals::Debug = true;
	else
		Globals::Debug = false;
}

void cmd_init(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos
		;

	po::options_description desc;
	desc.add_options()
	("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		;

	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	if (!fs::exists(Globals::SimpleGitDir))
	{
		init_filesystem();
		message("initialized empty git repository in " + fs::absolute(Globals::SimpleGitDir).string());
	}
	else
		error("git repository already exists: " + fs::absolute(Globals::SimpleGitDir).string());
}

void cmd_add(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos
		.add("files", -1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("files", po::value<std::vector<fs::path>>(), "files to update in the index")
		;

	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	if (!vm.count("files"))
		message("nothing specified, nothing added");
	else
	{
		std::vector<fs::path> files = vm["files"].as<std::vector<fs::path>>();

		std::sort(files.begin(), files.end());
		update_index(files, UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD | UPDATE_INDEX_REMOVE_IF_DELETED);
	}
}

void cmd_update_index(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos
		.add("files", -1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("add", po::value<bool>()->implicit_value(true)->zero_tokens(), "add files even if not in index")
		("remove", po::value<bool>()->implicit_value(true)->zero_tokens(), "remove files if missing")
		("force-remove", po::value<bool>()->implicit_value(true)->zero_tokens(), "remove files even if not missing")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	int flags = UPDATE_INDEX_MODIFY;

	if (vm.count("add"))
		flags = UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD;
	else if (vm.count("remove"))
		flags = UPDATE_INDEX_REMOVE;
	else if (vm.count("force-remove"))
		flags = UPDATE_INDEX_FORCE_REMOVE;

	update_index(vm["files"].as<std::vector<fs::path>>(), flags);
}

void cmd_commit(int argc, char* argv[])
{
	po::positional_options_description pos;

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("message,m", po::value<std::string>(), "commit message")
		("file,F", po::value<std::string>(), "commit message file")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();
	if (Globals::Config.find("user.name") == Globals::Config.end())
		throw Exception("user name not defined");

	std::string tree_id = write_tree(); // TODO: check if anything changed

	Commit commit;
	commit.tree_id = tree_id;
	commit.committer = Globals::Config["user.name"];
	commit.author = Globals::Config["user.name"];

	if (vm.count("message"))
		commit.message = vm["message"].as<std::string>();
	else if (vm.count("file"))
		commit.message = Filesystem::read_content(vm["file"].as<std::string>());
	else
		commit.message = get_commit_message();

	std::string one_line_commit_message = commit.message;
	std::replace(one_line_commit_message.begin(), one_line_commit_message.end(), '\n', ' ');

	bool root_commit = !fs::exists(Globals::HeadFile);
	bool not_detached = true;
	std::string head;

	if (!root_commit)
	{
		head = Filesystem::read_content(Globals::HeadFile);
		not_detached = is_branch(head);
		if (not_detached)
			commit.parents = { resolve_branch(head) };
		else
			commit.parents = { head }; // head is detached
	}

	std::string commit_id = write_commit(commit);

	if (!root_commit && not_detached)
	{
		write_branch(head, commit_id, Filesystem::FILE_FLAG_OVERWRITE);
		message(boost::format("[%1% %2%] %3%") % head % commit_id % one_line_commit_message);
	}
	else if (root_commit)
	{
		write_branch("master", commit_id, Filesystem::FILE_FLAG_OVERWRITE);
		write_head("master");
		message(boost::format("[master (root-commit) %1%] %2%") % commit_id % one_line_commit_message);
	}
	else if (!not_detached)
	{
		write_head(commit_id);
		message(boost::format("[detached HEAD %1%] %2%") % commit_id % one_line_commit_message);
	}
}

void cmd_tag(int argc, char* argv[])
{
	/////////////////////////////////////////////////////////////////////////
	//	parse command line
	/////////////////////////////////////////////////////////////////////////
	po::options_description root_desc;
	root_desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("delete,d", po::value<bool>()->implicit_value(true)->zero_tokens(), "delete tag")
		("list,", po::value<bool>()->implicit_value(true)->zero_tokens(), "list tags")
		("force,f", po::value<bool>()->implicit_value(true)->zero_tokens(), "overwrites tag if exists")
		;

	po::variables_map root_vm;
	po::store(po::command_line_parser(argc, argv).
		options(root_desc).allow_unregistered().run(), root_vm);

	if (root_vm.count("-D"))
	{
		root_vm.insert(std::make_pair("delete", po::variable_value(true, true)));
		root_vm.insert(std::make_pair("force", po::variable_value(true, true)));
	}

	po::variables_map vm;
	po::positional_options_description pos;
	po::options_description desc;
	desc.add(root_desc);

	// define options depending on the subgroup
	if (root_vm.count("delete"))
	{
		pos
			.add("tagname", -1)
			;
		desc.add_options()
			("tagname", po::value<std::string>())
			;
	}
	else if (root_vm.count("list"))
	{
		pos
			;
		desc.add_options()
			;
	}
	else
	{
		pos
			.add("tagname", 1)
			.add("commit", 1)
			;
		desc.add_options()
			("tagname", po::value<std::string>())
			("commit", po::value<std::string>())
			;
	}

	po::command_line_parser actual_parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	po::store(actual_parser.run(), vm);
	vm.insert(root_vm.begin(), root_vm.end());
	po::notify(root_vm);
	/////////////////////////////////////////////////////////////////////////

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	if (vm.count("list"))
	{
		list_tags();
	}
	if (vm.count("delete"))
	{
		delete_tag(vm["tagname"].as<std::string>());

	}
	else if (vm.count("tagname"))
	{
		int flags = vm.count("force") ? Filesystem::FILE_FLAG_OVERWRITE : 0;
		std::string commit = vm.count("commit") ? resolve_ref(vm["commit"].as<std::string>()) : resolve_head();
		write_tag(vm["tagname"].as<std::string>(), commit, flags);
	}
}

void cmd_read_tree(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("tree-ish", 3);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		(",m", po::value<bool>()->implicit_value(true)->zero_tokens(), "perform merge")
		("tree-ish", po::wvalue<std::vector<std::string>>(), "list of tree objects")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	if (!vm.count("tree-ish"))
		error("expected tree id");

	if (vm.count("-m"))
	{
		std::vector<std::string> tree_ish = vm["tree-ish"].as<std::vector<std::string>>();
		if (tree_ish.size() == 1)
			read_tree_into_index(Globals::IndexFile, tree_ish[0], false);
		if (tree_ish.size() == 2)
			read_tree_into_index(Globals::IndexFile,
				resolve_to_tree(tree_ish[0]),
				resolve_to_tree(tree_ish[1]));
		else
			;
	}
	else
		resolve_to_tree(vm["tree-ish"].as<std::vector<std::string>>()[0]);
}

void cmd_write_tree(int argc, char* argv[])
{
	po::positional_options_description pos;

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("missing-ok", po::value<bool>()->implicit_value(true), "adds blob to the tree even if it doesn't exist")
		("prefix", po::value<fs::path>())
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	fs::path prefix = vm.count("prefix") ? vm["prefix"].as<fs::path>() : "";
	bool ignore_missing = vm.count("missing-ok");

	message(write_tree(prefix, ignore_missing));
}

void cmd_branch(int argc, char* argv[])
{
	/////////////////////////////////////////////////////////////////////////
	//	parse command line
	/////////////////////////////////////////////////////////////////////////
	po::options_description root_desc;
	root_desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("delete,d", po::value<bool>()->implicit_value(true)->zero_tokens())
		("move,m", po::value<bool>()->implicit_value(true)->zero_tokens())
		("copy,c", po::value<bool>()->implicit_value(true)->zero_tokens())
		("list,", po::value<bool>()->implicit_value(true)->zero_tokens())
		(",D", po::value<bool>()->implicit_value(true)->zero_tokens())
		(",M", po::value<bool>()->implicit_value(true)->zero_tokens())
		(",C", po::value<bool>()->implicit_value(true)->zero_tokens())
		("force,f", po::value<bool>()->implicit_value(true)->zero_tokens())
		;

	po::variables_map root_vm;
	po::store(po::command_line_parser(argc, argv).
		options(root_desc).allow_unregistered().run(), root_vm);

	if (root_vm.count("-D"))
	{
		root_vm.insert(std::make_pair("delete", po::variable_value(true, true)));
		root_vm.insert(std::make_pair("force", po::variable_value(true, true)));
	}
	if (root_vm.count("-M"))
	{
		root_vm.insert(std::make_pair("move", po::variable_value(true, true)));
		root_vm.insert(std::make_pair("force", po::variable_value(true, true)));
	}
	if (root_vm.count("-C"))
	{
		root_vm.insert(std::make_pair("copy", po::variable_value(true, true)));
		root_vm.insert(std::make_pair("force", po::variable_value(true, true)));
	}

	po::variables_map vm;
	po::positional_options_description pos;
	po::options_description desc;
	desc.add(root_desc);

	// define options depending on the subgroup
	if (root_vm.count("delete"))
	{
		pos
			.add("branchname", -1)
			;
		desc.add_options()
			("branchname", po::wvalue<std::vector<std::string>>())
			;
	}
	else if (root_vm.count("move"))
	{

		pos
			.add("oldbranch", 1)
			.add("newbranch", 1)
			;
		desc.add_options()
			("oldbranch", po::wvalue<std::string>())
			("newbranch", po::wvalue<std::string>())
			;
	}
	else if (root_vm.count("copy"))
	{
		pos
			.add("oldbranch", 1)
			.add("newbranch", 1)
			;
		desc.add_options()
			("oldbranch", po::wvalue<std::string>())
			("newbranch", po::wvalue<std::string>())
			;
	}
	else if (root_vm.count("list"))
	{
		pos
			;
		desc.add_options()
			;
	}
	else
	{
		pos
			.add("branchname", 1)
			.add("start-point", 1)
			;
		desc.add_options()
			("branchname", po::wvalue<std::string>())
			("start-point", po::wvalue<std::string>())
			;
	}

	po::command_line_parser actual_parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	po::store(actual_parser.run(), vm);
	vm.insert(root_vm.begin(), root_vm.end());
	po::notify(root_vm);
	/////////////////////////////////////////////////////////////////////////

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	int flags = vm.count("force") ? Filesystem::FILE_FLAG_OVERWRITE : 0;

	if (vm.count("list"))
	{
		list_branches();
	}
	else if (vm.count("delete"))
	{
		for (const std::string& branch : vm["branchname"].as<std::vector<std::string>>())
			delete_branch(branch, flags);
	}
	else if (vm.count("move"))
	{
		move_branch(vm["oldbranch"].as<std::string>(), vm["newbranch"].as<std::string>(), flags);
	}
	else if (vm.count("copy"))
	{
		copy_branch(vm["oldbranch"].as<std::string>(), vm["newbranch"].as<std::string>(), flags);
	}
	else if (vm.count("branchname"))
	{
		std::string commit = vm.count("start-point") ? resolve_ref(vm["commit"].as<std::string>()) : resolve_head();
		write_branch(vm["branchname"].as<std::string>(), commit, flags);
	}
}

void cmd_stash(int argc, char* argv[])
{
	error("command not implemented: stash");
}

void cmd_checkout(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("commit-id", 1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("commit-id", po::wvalue<std::string>())
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	if (!vm.count("commit-id"))
		error("expected commit id");

	const std::string name = vm["commit-id"].as<std::string>();
	std::string commit_id = resolve_branch(name);
	checkout(commit_id);
	write_head(name);
}

void cmd_merge(int argc, char* argv[])
{
	error("command not implemented: merge");
}

void cmd_cat_file(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("object", 1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("object", po::value<std::string>()->required())
		(",t", po::value<bool>()->implicit_value(true)->zero_tokens())
		(",s", po::value<bool>()->implicit_value(true)->zero_tokens())
		(",p", po::value<bool>()->implicit_value(true)->zero_tokens())
		;

	po::variables_map vm;
	po::store(po::basic_command_line_parser<char>(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	conflicting_options(vm, "-t", "-p");
	conflicting_options(vm, "-t", "-s");
	conflicting_options(vm, "-p", "-s");

	at_least_one_required(vm, { "-t", "-p", "-s"});

	init_for_git_commands();

	const std::string object_id = resolve_ref(vm["object"].as<std::string>());

	if (vm.count("-t"))
		message(Object(object_id).kind());
	else if (vm.count("-s"))
		message(Object(object_id).size());
	else if (vm.count("-p"))
		Object(object_id).get_reader()->pretty_print(std::cout);
}

void cmd_ls_files(int argc, char* argv[])
{
	po::positional_options_description pos;

	po::options_description desc;
	desc.add_options()
	("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		;

	po::variables_map vm;
	po::store(po::basic_command_line_parser<char>(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	if (!fs::exists(Globals::IndexFile))
		return;

	std::ifstream in_stream;
	Filesystem::open(Globals::IndexFile, in_stream);

	IndexRecord record;
	while (in_stream >> record)
		std::cout << record.path.string() << '\n';
}


void cmd_config(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("name", 1);
	pos.add("value", 1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("name", po::value<std::string>()->required())
		("value", po::value<std::string>()->default_value(""))
		;

	po::variables_map vm;
	po::store(po::basic_command_line_parser<char>(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	write_config(Globals::ConfigFile, vm["name"].as<std::string>(), vm["value"].as<std::string>());
}

void cmd_diff(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("args", -1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("args", po::value<std::vector<std::string>>())
		;

	po::positional_options_description pos_blob;
	pos_blob.add("blob", 2);

	po::options_description desc_blob;
	desc_blob.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("blob", po::value<std::vector<std::string>>())
		;
	
	po::positional_options_description pos_treeish;
	pos_treeish.add("tree-ish", 2);

	po::options_description desc_treeish;
	desc_treeish.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("tree-ish", po::value<std::vector<std::string>>()->required())
		;

	po::variables_map vm;
	po::store(po::basic_command_line_parser<char>(argc, argv)
		.options(desc)
		.positional(pos)
		.allow_unregistered()
		.run(), vm);

	po::notify(vm);

	init_for_git_commands();

	// if the index file doesn't exist, we have nothing to do
	if (!fs::exists(Globals::IndexFile))
		return;

	std::vector<std::string> args = vm.count("args") ? vm["args"].as<std::vector<std::string>>() : std::vector<std::string>();
	if (args.size() == 2)
	{
		std::string id1, id2;
		if (try_resolve_to_blob(args[0], id1) && try_resolve_to_blob(args[1], id2))
		{
			po::variables_map vm;
			po::store(po::basic_command_line_parser<char>(argc, argv)
				.options(desc_blob)
				.positional(pos_blob).run(), vm);
			po::notify(vm);


			Diff d(Object(id1).get_blob_reader()->read_lines(),
				Object(id2).get_blob_reader()->read_lines());

			d.calculate();
			d.print();
		}
		else if (try_resolve_to_tree(args[0], id1) && try_resolve_to_tree(args[1], id2))
		{
			po::variables_map vm;
			po::store(po::basic_command_line_parser<char>(argc, argv)
				.options(desc_treeish)
				.positional(pos_treeish).run(), vm);
			po::notify(vm);

			return diff_tree(id1, id2);
		}
	}
	else if (std::all_of(args.begin(), args.end(), [](const fs::path& path) {return fs::exists(path); }))
		diff_index(Globals::IndexFile, std::vector<fs::path>(args.begin(), args.end()));
	else
		error("invalid combination of positional arguments");
}

void cmd_reset(int argc, char* argv[])
{
	po::positional_options_description pos;
	pos.add("commit", 1);

	po::options_description desc;
	desc.add_options()
		("help,h", po::value<bool>()->implicit_value(true)->zero_tokens(), "prints command usage")
		("soft", po::value<bool>()->implicit_value(true)->zero_tokens(), "does not modify the working tree or the index")
		("mixed", po::value<bool>()->implicit_value(true)->zero_tokens(), "resets the index but not the working tree")
		("hard", po::value<bool>()->implicit_value(true)->zero_tokens(), "resets the index and the working tree")
		("commit", po::value<std::string>(), "target commit")
		;

	po::variables_map vm;
	po::store(po::basic_command_line_parser<char>(argc, argv)
		.options(desc)
		.positional(pos).run(), vm);
	po::notify(vm);

	conflicting_options(vm, "soft", "mixed");
	conflicting_options(vm, "mixed", "hard");
	conflicting_options(vm, "soft", "hard");

	at_least_one_required(vm, { "soft", "mixed", "hard" });

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	init_for_git_commands();

	std::string commit_id = resolve_to_commit(vm["commit"].as<std::string>());

	bool keep_index = vm.count("soft");
	bool keep_working_directory = !vm.count("hard");

	checkout(commit_id, keep_working_directory, keep_index);

	Commit commit;
	*(Object(commit_id).get_commit_reader()) >> commit;

	std::cout << "HEAD is now at " << commit_id << " " << commit.message << "\n";
}

void cmd_test(int argc, char* argv[])
{

}