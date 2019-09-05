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

std::map<std::string, void (*)(int argc, char* argv[])> command_map {
	{"init", cmd_init},
	{"add", cmd_add},
	{"update-index", cmd_update_index},
	{"commit", cmd_commit},
	{"tag", cmd_tag},
	{"branch", cmd_branch},
	{"stash", cmd_stash},
	{"checkout", cmd_checkout},
	{"merge", cmd_merge},
	{"read-tree", cmd_read_tree},
	{"write-tree", cmd_write_tree},
	{"cat-file", cmd_cat_file},
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
	Globals::DefaultSimpleGitDir = ".simplegit";
	const char* gitdir = std::getenv("SIMPLEGIT_DIR");
	if (gitdir)
		Globals::SimpleGitDir = to_wide_string(gitdir);
	else
		Globals::SimpleGitDir = Globals::DefaultSimpleGitDir;

	Globals::DefaultSimpleGitConfig = Globals::SimpleGitDir / "config";
	const char* config = std::getenv("SIMPLEGIT_CONFIG");
	if (config)
		Globals::ConfigFile = Globals::SimpleGitDir / to_wide_string(config);
	else
		Globals::ConfigFile = Globals::DefaultSimpleGitConfig;

	Globals::ObjectDir = Globals::SimpleGitDir / "objects";
	Globals::RefDir = Globals::SimpleGitDir / "refs";
	Globals::TagDir = Globals::RefDir / "tags";
	Globals::BranchDir = Globals::RefDir / "heads";

	Globals::IndexFile = Globals::SimpleGitDir / "INDEX";
	Globals::HeadFile = Globals::SimpleGitDir / "HEAD";
}

#include <fstream>
int main(int argc, char* argv[])
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
			std::string command = argv[1];
			auto cmd = command_map.find(command);
			if (cmd != command_map.end())
				try{
					cmd->second(argc - 1, argv + 1);
				} catch (const std::exception& e) {
					error(e.what());
				}
			else
				error(std::string("unknown command: ") + command);
		}

		return 0;
	}
	catch (const boost::program_options::unknown_option& e)
	{
		error("unknown option: %s", e.what());
	}
}