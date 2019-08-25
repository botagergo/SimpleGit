#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>

#include <windows.h>

#include "blob.h"
#include "commands.h"
#include "config.h"
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
	{L"read-tree", cmd_read_tree},
};

void clear_filesystem()
{
	RemoveDirectory(Globals::SimpleGitDir.c_str());
}

int test()
{
	return 0;
}

void initPathConstants()
{
	Globals::DefaultSimpleGitDir = L".simplegit";
	const char* gitdir = std::getenv("SIMPLEGIT_DIR");
	if (gitdir)
		Globals::SimpleGitDir = to_wide_string(gitdir);
	else
		Globals::SimpleGitDir = Globals::DefaultSimpleGitDir;

	Globals::DefaultSimpleGitConfig = Globals::SimpleGitDir / L"config";
	const char* config = std::getenv("SIMPLEGIT_CONFIG");
	if (config)
		Globals::ConfigFile = Globals::SimpleGitDir / to_wide_string(config);
	else
		Globals::ConfigFile = Globals::DefaultSimpleGitConfig;

	Globals::ObjectDir = Globals::SimpleGitDir / L"objects";
	Globals::RefDir = Globals::SimpleGitDir / L"refs";
	Globals::TagDir = Globals::RefDir / L"tags";
	Globals::BranchDir = Globals::RefDir / L"heads";

	Globals::IndexFile = Globals::SimpleGitDir / L"INDEX";
	Globals::HeadFile = Globals::SimpleGitDir / L"HEAD";
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