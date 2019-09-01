#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>

#include <windows.h>

#include <boost/program_options/errors.hpp>

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
//#define WAIT_FOR_ENTER

std::map<std::wstring, void (*)(int argc, wchar_t* argv[])> command_map {
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
	{L"write-tree", cmd_write_tree},
	{L"cat-file", cmd_cat_file},
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
int wmain(int argc, wchar_t* argv[])
{
#ifdef WAIT_FOR_ENTER
	{std::string str; std::cin >> str; }
#endif
	initPathConstants();

	try {
		if (argc == 1)
			;
		else
		{
			std::wstring command = argv[1];
			auto cmd = command_map.find(command);
			if (cmd != command_map.end())
				try{
					cmd->second(argc - 1, argv + 1);
				} catch (const std::exception& e) {
					error(e.what());
				}
			else
				error(std::wstring(L"unknown command: ") + command);
		}

		return 0;
	}
	catch (const boost::program_options::unknown_option& e)
	{
		error(L"unknown option: %s", e.what());
	}
}