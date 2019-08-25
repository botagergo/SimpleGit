#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
#include "tag.h"
#include "tree.h"

void init_filesystem()
{
	Filesystem::create_directory(Globals::SimpleGitDir, true);
	Filesystem::create_directory(Globals::ObjectDir);
	Filesystem::create_directory(Globals::RefDir);
	Filesystem::create_directory(Globals::TagDir);
	Filesystem::create_directory(Globals::BranchDir);

	Filesystem::write_content(Globals::ConfigFile, L"", Filesystem::FILE_FLAG_OVERWRITE);
	Filesystem::write_content(Globals::HeadFile, L"master", Filesystem::FILE_FLAG_OVERWRITE);
}

// initializes stuff that are needed for the actual repository commands
void init_for_git_commands()
{
	Globals::Config = read_config(Globals::ConfigFile);
}

void cmd_init(const std::vector<std::wstring>& args)
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
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_add(const std::vector<std::wstring>& args)
{
	try {
		init_for_git_commands();

		std::vector<fs::path> files;
		for (int i = 0; i < args.size(); i++)
		{
			if (args[i][0] == '-')
				;
			else
				files.push_back(args[i]);
		}

		if (files.empty())
			error(L"expected file names");

		std::sort(files.begin(), files.end());
		update_index(files, UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD);
	}
	catch (const Exception &e) {
		error(e.message());
	}
}

void cmd_update_index(const std::vector<std::wstring>& args)
{
	try {
		init_for_git_commands();

		std::vector<fs::path> files;
		int flags = UPDATE_INDEX_MODIFY;

		for (int i = 0; i < args.size(); i++)
		{
			if (args[i][0] == '-')
			{
				if (args[i] == L"--add")
					flags = UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD;
				else if (args[i] == L"--remove")
					flags = UPDATE_INDEX_REMOVE;
				else if (args[i] == L"--force-remove")
					flags = UPDATE_INDEX_REMOVE | UPDATE_INDEX_FORCE_REMOVE;
			}
			else
				files.push_back(args[i]);
		}

		if (files.empty())
			error(L"expected file names");

		update_index(files, flags);
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_commit(const std::vector<std::wstring>& args)
{
	try{
		init_for_git_commands();

		std::wstring message;

		for (int i = 0; i < args.size(); i++)
		{
			if (args[i] == L"-m")
			{
				message = args[i + 1];
				i++;
			}
		}

		if (message.empty())
			error(L"you have to specify a commit message");

		std::wstring tree_id = write_tree();
		std::wstring commit_id = write_commit(tree_id, std::vector<std::wstring>(), Globals::Config[L"user.name"], Globals::Config[L"user.name"], message);
		//checkout(commit_id);
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_tag(const std::vector<std::wstring>& args)
{
	try {
		init_for_git_commands();

		if (args.size() == 0)
			;
		else if (args[0] == L"-d" || args[0] == L"--delete")
		{
			if (args.size() < 2)
				error(L"expected tag names");

			for (int i = 1; i < args.size(); i++)
				delete_tag(args[i]);
		}
		else
		{
			if (args.size() < 3)
				error(L"expected tag name and referenced object id");

			create_tag(args[1], args[2], args.size() > 3 && (args[3] == L"-f" || args[3] == L"--force") ? Filesystem::FILE_FLAG_OVERWRITE : 0);
		}
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_read_tree(const std::vector<std::wstring>& args)
{
	try {
		init_for_git_commands();

		if (args.size() < 1)
			error(L"expected tree id");

		write_index(Globals::IndexFile, args[0]);
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_branch(const std::vector<std::wstring>& args)
{
	error(L"command not implemented: branch");
}

void cmd_stash(const std::vector<std::wstring>& args)
{
	error(L"command not implemented: stash");
}

void cmd_checkout(const std::vector<std::wstring>& args)
{
	try {
		init_for_git_commands();

		if (args.size() < 1)
			error(L"expected commit id");

		Commit commit = read_commit(args[0]);
		set_working_directory(commit.tree_id);
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_merge(const std::vector<std::wstring>& args)
{
	error(L"command not implemented: merge");
}
