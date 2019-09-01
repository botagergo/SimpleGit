#include <filesystem>

#include "checkout.h"
#include "commit.h"
#include "globals.h"
#include "index.h"
#include "tree.h"

void clear_working_directory()
{
	for (const fs::path& path : fs::directory_iterator(L"."))
	{
		if(!fs::equivalent(path, Globals::SimpleGitDir))
			fs::remove_all(path);
	}
}

void set_working_directory(const std::wstring& tree_id)
{
	clear_working_directory();
	read_tree(tree_id);
}

void checkout(const std::wstring& commit_id)
{
	Commit commit = read_commit(commit_id);
	read_tree_into_index(Globals::IndexFile, commit.tree_id);
	set_working_directory(commit.tree_id);
}
