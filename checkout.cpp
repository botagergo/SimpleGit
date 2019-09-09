#include <filesystem>

#include "checkout.h"
#include "commit.h"
#include "globals.h"
#include "index.h"
#include "tree.h"

void clear_working_directory()
{
	for (const fs::path& path : fs::directory_iterator("."))
	{
		if(!fs::equivalent(path, Globals::SimpleGitDir))
			fs::remove_all(path);
	}
}

void set_working_directory(const std::string& tree_id)
{
	clear_working_directory();
	read_tree(tree_id);
}

void checkout(const std::string& commit_id)
{
	Commit commit;
	*Object(commit_id).get_commit_reader() >> commit;
	read_tree_into_index(Globals::IndexFile, commit.tree_id);
	set_working_directory(commit.tree_id);
}
