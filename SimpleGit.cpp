#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>

#include <windows.h>

#include "blob.h"
#include "commands.h"
#include "error.h"
#include "exception.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "ref.h"
#include "tag.h"

//#define TEST

std::map<std::wstring, void (*)(const std::vector<std::wstring>&)> command_map{
	{L"init", cmd_init},
	{L"add", cmd_add},
	{L"update-index", cmd_update_index},
	{L"commit", cmd_commit},
	{L"tag", cmd_tag},
	{L"branch", cmd_branch},
	{L"stash", cmd_stash},
	{L"checkout", cmd_checkout},
	{L"merge", cmd_merge},
};

void clear_filesystem()
{
	RemoveDirectory(Config::SimpleGitDir.c_str());
}

int test()
{
	return 0;
}

void initPathConstants()
{
	const char* gitdir = std::getenv("SIMPLEGIT_DIR");
	if (gitdir)
		Config::SimpleGitDir = to_wide_string(gitdir);
	else
		Config::SimpleGitDir = Config::DefaultSimpleGitDir;

	Config::ObjectDir = Config::SimpleGitDir / L"objects";
	Config::RefDir = Config::SimpleGitDir / L"refs";
	Config::TagDir = Config::RefDir / L"tags";
	Config::BranchDir = Config::RefDir / L"heads";

	Config::IndexFile = Config::SimpleGitDir / L"INDEX";
}

#include <fstream>
int main(int argc, char *argv[])
{
	initPathConstants();

	if (argc == 1)
		;
	else
	{
		std::vector<std::wstring> wargs;
		std::wstring command = to_wide_string(argv[1]);

		for (int i = 2; i < argc; i++)
		{
			wargs.push_back(to_wide_string(argv[i]));
		}

		auto cmd = command_map.find(command);
		if (cmd != command_map.end())
			cmd->second(wargs);
	}

	return 0;
}