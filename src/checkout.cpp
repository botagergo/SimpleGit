#include "filesystem.h"

#include "checkout.h"
#include "commit.h"
#include "globals.h"
#include "index.h"
#include "repository.h"
#include "tree.h"

void clear_working_directory(const Repository& repo)
{
	for (const fs::path& path : fs::directory_iterator("."))
	{
		if(!fs::equivalent(path, repo.gitDir))
			fs::remove_all(path);
	}
}

void set_working_directory(const Repository& repo, const std::string& tree_id)
{
	clear_working_directory(repo);
	read_tree(repo, tree_id);
}

void checkout(const Repository& repo, const std::string& commit_id, bool keep_working_directory, bool keep_index)
{
	Commit commit;
	*Object(repo, commit_id).get_commit_reader() >> commit;

	if (!keep_index)
		read_tree_into_index(repo, repo.indexFile, commit.tree_id);
	if (!keep_working_directory)
		set_working_directory(repo, commit.tree_id);
}
