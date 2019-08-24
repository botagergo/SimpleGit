#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "commands.h"
#include "commit.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "index.h"
#include "tag.h"
#include "tree.h"

void init_filesystem()
{
	Filesystem::create_directory(Config::SimpleGitDir, true);
	Filesystem::create_directory(Config::ObjectDir);
	Filesystem::create_directory(Config::RefDir);
	Filesystem::create_directory(Config::TagDir);
	Filesystem::create_directory(Config::BranchDir);
}

void cmd_init(const std::vector<std::wstring>& args)
{
	try {
		if (!std::filesystem::exists(Config::SimpleGitDir))
		{
			init_filesystem();
			message(L"Initialized empty Git repository in " + std::filesystem::absolute(Config::SimpleGitDir).wstring());
		}
		else
			error(L"git repository already exists: " + std::filesystem::absolute(Config::SimpleGitDir).wstring());
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_add(const std::vector<std::wstring>& args)
{
	std::vector<std::filesystem::path> files;
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

	try {
		update_index(files, UPDATE_INDEX_MODIFY | UPDATE_INDEX_ADD);
	}
	catch (const Exception &e) {
		error(e.message());
	}
}

void cmd_update_index(const std::vector<std::wstring>& args)
{
	std::vector<std::filesystem::path> files;
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

	try {
		update_index(files, flags);
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_commit(const std::vector<std::wstring>& args)
{
	try {
		std::wstring tree_id = write_tree();
		create_commit(tree_id, std::vector<std::wstring>(), L"gergo", L"gergo", L"message");
	}
	catch (Exception e) {
		error(e.message());
	}
}

void cmd_tag(const std::vector<std::wstring>& args)
{
	try {
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
	error(L"command not implemented: checkout");
}

void cmd_merge(const std::vector<std::wstring>& args)
{
	error(L"command not implemented: merge");
}
