#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

#include "branch.h"
#include "checkout.h"
#include "commands.h"
#include "commit.h"
#include "config.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "merge.h"
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
	Filesystem::create_directory(Globals::SimpleGitDir, true);
	Filesystem::create_directory(Globals::ObjectDir);
	Filesystem::create_directory(Globals::RefDir);
	Filesystem::create_directory(Globals::TagDir);
	Filesystem::create_directory(Globals::BranchDir);

	Filesystem::write_content(Globals::ConfigFile, L"", Filesystem::FILE_FLAG_OVERWRITE);
}

// initializes stuff that are needed for the actual repository commands
void init_for_git_commands()
{
	Globals::Config = read_config(Globals::ConfigFile);
}

void cmd_init(int argc, wchar_t* argv[])
{
	try {
		if (!fs::exists(Globals::SimpleGitDir))
		{
			init_filesystem();
			message(L"Initialized empty Git repository in " + fs::absolute(Globals::SimpleGitDir).wstring());
		}
		else
			error(L"git repository already exists: " + fs::absolute(Globals::SimpleGitDir).wstring());
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_add(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;
		pos
			.add("files", -1);

		po::options_description desc;
		desc.add_options()
			("files", po::value<std::vector<fs::path>>(), "files to add")
			;

		po::variables_map vm;

		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		if (!vm.count("files"))
			message(L"nothing specified, nothing added");
		else
		{
			std::vector<fs::path> files = vm["files"].as<std::vector<fs::path>>();

			std::sort(files.begin(), files.end());
			update_index(files, UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD | UPDATE_INDEX_REMOVE_IF_DELETED);
		}
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_update_index(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;
		pos
			.add("files", -1);

		po::options_description desc;
		desc.add_options()
			("add", po::value<bool>()->implicit_value(true)->zero_tokens(), "add files even if not in index")
			("remove", po::value<bool>()->implicit_value(true)->zero_tokens(), "remove files if missing")
			("force-remove", po::value<bool>()->implicit_value(true)->zero_tokens(), "remove files even if not missing")
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		int flags = UPDATE_INDEX_MODIFY;

		if (vm.count("add"))
			flags = UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD;
		else if (vm.count("remove"))
			flags = UPDATE_INDEX_REMOVE;
		else if (vm.count("force-remove"))
			flags = UPDATE_INDEX_REMOVE | UPDATE_INDEX_FORCE_REMOVE;

		update_index(vm["files"].as<std::vector<fs::path>>(), flags);
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_commit(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;

		po::options_description desc;
		desc.add_options()
			("message,m", po::wvalue<std::wstring>()->required(), "commit message")
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		std::wstring tree_id = write_tree(); // TODO: check if anything changed

		Commit commit;
		commit.tree_id = tree_id;
		commit.committer = Globals::Config[L"user.name"];
		commit.author = Globals::Config[L"user.name"];
		commit.message = vm["message"].as<std::wstring>();

		bool root_commit = !fs::exists(Globals::HeadFile);
		bool not_detached = true;
		std::wstring head;

		if (!root_commit)
		{
			head = Filesystem::read_content(Globals::HeadFile);
			not_detached = is_branch(head);
			if (not_detached)
				commit.parents = { resolve_branch(head) };
			else
				commit.parents = { head }; // head is detached
		}

		std::wstring commit_id = write_commit(commit);

		if (!root_commit && not_detached)
		{
			write_branch(head, commit_id, Filesystem::FILE_FLAG_OVERWRITE);
			message(boost::wformat(L"[%1% %2%] %3%") % head % commit_id % commit.message);
		}
		else if (root_commit)
		{
			write_branch(L"master", commit_id, Filesystem::FILE_FLAG_OVERWRITE);
			write_head(L"master");
			message(boost::wformat(L"[master (root-commit) %1%] %2%") % commit_id % commit.message);
		}
		else if (!not_detached)
		{
			write_head(commit_id);
			message(boost::wformat(L"[detached HEAD %1%] %2%") % commit_id % commit.message);
		}
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_tag(int argc, wchar_t* argv[])
{
	try{
		/////////////////////////////////////////////////////////////////////////
		//	parse command line
		/////////////////////////////////////////////////////////////////////////
		po::options_description root_desc;
		root_desc.add_options()
			("delete,d", po::value<bool>()->implicit_value(true)->zero_tokens())
			("list,l", po::value<bool>()->implicit_value(true)->zero_tokens())
			("force,f", po::value<bool>()->implicit_value(true)->zero_tokens())
			;

		po::variables_map root_vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv).
			options(root_desc).allow_unregistered().run(), root_vm);

		if (root_vm.count("-D"))
		{
			root_vm.insert(std::make_pair("delete", po::variable_value(true, true)));
			root_vm.insert(std::make_pair("force", po::variable_value(true, true)));
		}

		po::notify(root_vm);

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
				("tagname", po::wvalue<std::wstring>())
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
				("tagname", po::wvalue<std::wstring>())
				("commit", po::wvalue<std::wstring>())
				;
		}

		po::wcommand_line_parser actual_parser = po::basic_command_line_parser<wchar_t>(argc, argv).options(desc).positional(pos);
		po::store(actual_parser.run(), vm);
		vm.merge(root_vm);
		/////////////////////////////////////////////////////////////////////////

		init_for_git_commands();

		if (vm.count("list"))
		{
			list_tags();
		}
		if (vm.count("delete"))
		{
			delete_tag(vm["tagname"].as<std::wstring>());

		}
		else if (vm.count("tagname"))
		{
			int flags = vm.count("force") ? Filesystem::FILE_FLAG_OVERWRITE : 0;
			std::wstring commit = vm.count("commit") ? resolve_ref(vm["commit"].as<std::wstring>()) : resolve_head();
			write_tag(vm["tagname"].as<std::wstring>(), commit, flags);
		}
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_read_tree(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;
		pos.add("tree-ish", 3);

		po::options_description desc;
		desc.add_options()
			(",m", po::value<bool>()->implicit_value(true)->zero_tokens())
			("tree-ish", po::wvalue<std::vector<std::wstring>>(), "list of tree objects")
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		if (!vm.count("tree-ish"))
			error(L"expected tree id");

		if (vm.count("-m"))
		{
			std::vector<std::wstring> tree_ish = vm["tree-ish"].as<std::vector<std::wstring>>();
			if (tree_ish.size() == 1)
				read_tree_into_index(Globals::IndexFile, tree_ish[0]);
			if (tree_ish.size() == 2)
				read_tree_into_index(Globals::IndexFile,
					resolve_tree(tree_ish[0]),
					resolve_tree(tree_ish[1]));
			else
				;
		}
		else
			resolve_tree(vm["tree-ish"].as<std::vector<std::wstring>>()[0]);
	}
	catch (const std::exception& e) {

		error(e.what());
	}
}

void cmd_write_tree(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;

		po::options_description desc;
		desc.add_options()
			("missing-ok", po::value<bool>()->implicit_value(true))
			("prefix", po::value<fs::path>())
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		fs::path prefix = vm.count("prefix") ? vm["prefix"].as<fs::path>() : L"";
		bool ignore_missing = vm.count("missing-ok");

		message(write_tree(prefix, ignore_missing));
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_branch(int argc, wchar_t* argv[])
{
	try {
		/////////////////////////////////////////////////////////////////////////
		//	parse command line
		/////////////////////////////////////////////////////////////////////////
		po::options_description root_desc;
		root_desc.add_options()
			("delete,d", po::value<bool>()->implicit_value(true)->zero_tokens())
			("move,m", po::value<bool>()->implicit_value(true)->zero_tokens())
			("copy,c", po::value<bool>()->implicit_value(true)->zero_tokens())
			("list,l", po::value<bool>()->implicit_value(true)->zero_tokens())
			(",D", po::value<bool>()->implicit_value(true)->zero_tokens())
			(",M", po::value<bool>()->implicit_value(true)->zero_tokens())
			(",C", po::value<bool>()->implicit_value(true)->zero_tokens())
			("force,f", po::value<bool>()->implicit_value(true)->zero_tokens())
			;

		po::variables_map root_vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv).
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

		po::notify(root_vm);

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
				("branchname", po::wvalue<std::vector<std::wstring>>())
				;
		}
		else if (root_vm.count("move"))
		{

			pos
				.add("oldbranch", 1)
				.add("newbranch", 1)
				;
			desc.add_options()
				("oldbranch", po::wvalue<std::wstring>())
				("newbranch", po::wvalue<std::wstring>())
				;
		}
		else if (root_vm.count("copy"))
		{
			pos
				.add("oldbranch", 1)
				.add("newbranch", 1)
				;
			desc.add_options()
				("oldbranch", po::wvalue<std::wstring>())
				("newbranch", po::wvalue<std::wstring>())
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
				("branchname", po::wvalue<std::wstring>())
				("start-point", po::wvalue<std::wstring>())
				;
		}

		po::wcommand_line_parser actual_parser = po::basic_command_line_parser<wchar_t>(argc, argv).options(desc).positional(pos);
		po::store(actual_parser.run(), vm);
		vm.merge(root_vm);
		/////////////////////////////////////////////////////////////////////////

		init_for_git_commands();

		int flags = vm.count("force") ? Filesystem::FILE_FLAG_OVERWRITE : 0;

		if (vm.count("list"))
		{
			list_branches();
		}
		else if (vm.count("delete"))
		{
			for (const std::wstring& branch : vm["branchname"].as<std::vector<std::wstring>>())
				delete_branch(branch, flags);
		}
		else if (vm.count("move"))
		{
			move_branch(vm["oldbranch"].as<std::wstring>(), vm["newbranch"].as<std::wstring>(), flags);
		}
		else if (vm.count("copy"))
		{
			copy_branch(vm["oldbranch"].as<std::wstring>(), vm["newbranch"].as<std::wstring>(), flags);
		}
		else if (vm.count("branchname"))
		{
			std::wstring commit = vm.count("start-point") ? resolve_ref(vm["commit"].as<std::wstring>()) : resolve_head();
			write_branch(vm["branchname"].as<std::wstring>(), commit, flags);
		}
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_stash(int argc, wchar_t* argv[])
{
	error(L"command not implemented: stash");
}

void cmd_checkout(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;
		pos.add("commit-id", 1);

		po::options_description desc;
		desc.add_options()
			("commit-id", po::wvalue<std::wstring>())
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		init_for_git_commands();

		if (!vm.count("commit-id"))
			error(L"expected commit id");

		const std::wstring name = vm["commit-id"].as<std::wstring>();
		std::wstring commit_id = resolve_branch(name);
		checkout(commit_id);
		write_head(name);
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}

void cmd_merge(int argc, wchar_t* argv[])
{
	error(L"command not implemented: merge");
}

void cmd_cat_file(int argc, wchar_t* argv[])
{
	try {
		po::positional_options_description pos;
		pos.add("object", 1);

		po::options_description desc;
		desc.add_options()
			("object", po::wvalue<std::wstring>()->required())
			(",t", po::value<bool>()->implicit_value(true)->zero_tokens())
			(",p", po::value<bool>()->implicit_value(true)->zero_tokens())
			;

		po::variables_map vm;
		po::store(po::basic_command_line_parser<wchar_t>(argc, argv)
			.options(desc)
			.positional(pos).run(), vm);
		po::notify(vm);

		conflicting_options(vm, "-t", "-p");
		at_least_one_required(vm, { "-t", "-p" });

		init_for_git_commands();

		const std::wstring object_id = resolve_ref(vm["object"].as<std::wstring>());

		if (vm.count("-t"))
			message(get_object_type(object_id));
		else if (vm.count("-p"))
			pretty_print_object(std::wcout, object_id);
	}
	catch (const std::exception& e) {
		error(e.what());
	}
}