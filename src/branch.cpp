#include <boost/filesystem.hpp>

#include "branch.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "repository.h"

namespace fs = boost::filesystem;

void write_branch(const Repository& repo, const std::string& name, const std::string& object_id, int flags)
{
	try {
		Filesystem::write_content(repo.branchDir / name, object_id, flags | Filesystem::FILE_FLAG_CREATE_DIRECTORY);
	}
	catch (const FileExistsException&) {
		throw BranchExistsException(name);
	}
}

void move_branch(const Repository& repo, const std::string& oldname, const std::string& newname, int flags)
{
	if (!is_branch(repo, oldname))
		throw Exception(boost::format("branch doesn't exist: %1%") % oldname);

	if (is_branch(repo, newname))
	{
		if (flags & Filesystem::FILE_FLAG_OVERWRITE)
			delete_branch(repo, newname);
		else
			throw BranchExistsException(newname);
	}

	if (read_head(repo) == oldname)
		write_head(repo, newname);

	fs::rename(repo.branchDir / oldname, repo.branchDir / newname);
}

void copy_branch(const Repository& repo, const std::string& oldname, const std::string& newname, int flags)
{
	if (!is_branch(repo, oldname))
		throw Exception(boost::format("branch doesn't exist: %1%") % oldname);

	if (is_branch(repo, newname))
	{
		if (flags & Filesystem::FILE_FLAG_OVERWRITE)
			delete_branch(repo, newname);
		else
			throw BranchExistsException(newname);
	}

	fs::copy(repo.branchDir / oldname, repo.branchDir / newname);
}

void delete_branch(const Repository& repo, const std::string& name, int flags)
{
	if (read_head(repo) == name)
		throw Exception(boost::format("cannot delete branch '%1%', checked out") % name);
	if (!fs::remove(repo.branchDir / name))
		throw Exception(boost::format("branch doesn't exist: %1%") % name);
}

bool is_branch(const Repository& repo, const std::string& name)
{
	return fs::exists(repo.branchDir / name);
}

void write_head(const Repository& repo, const std::string& name)
{
	Filesystem::write_content(repo.headFile, name, Filesystem::FILE_FLAG_OVERWRITE);
}

std::string read_head(const Repository& repo)
{
	return Filesystem::read_content(repo.headFile);
}

void list_branches(const Repository& repo)
{
	if (!fs::exists(repo.branchDir))
		return;

	for (const fs::path branch : fs::directory_iterator(repo.branchDir))
	{
		std::cout << branch.filename().string() << '\n';
	}
}
