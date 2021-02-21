#include <iostream>
#include <map>
#include <sstream>

#include "boost/program_options/errors.hpp"
#include "boost/dll.hpp"

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
	{"ls-files", cmd_ls_files},
	{"config", cmd_config},
	{"diff", cmd_diff},
	{"reset", cmd_reset},
	{"test", cmd_test},
};

void clear_filesystem()
{
	fs::remove_all(Globals::GitDir);
}

int test()
{
	return 0;
}

#include <fstream>
int main(int argc, char* argv[])
{
#ifdef WAIT_FOR_ENTER
	{std::string str; std::cin >> str; }
#endif

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
					FATAL << e.what() << std::endl;
					std::exit(1);
				}
			else
			{
				FATAL << command << " is not a git command" << std::endl;
				std::exit(1);
			}
		}

		return 0;
	}
	catch (const boost::program_options::unknown_option& e)
	{
		error("unknown option: %s", e.what());
	}
}
