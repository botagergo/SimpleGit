#include <boost/filesystem.hpp>

#include "branch.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"

namespace fs = boost::filesystem;

void write_branch(const std::string& name, const std::string& object_id, int flags)
{
	try {
		Filesystem::write_content(Globals::BranchDir / name, object_id, flags | Filesystem::FILE_FLAG_CREATE_DIRECTORY);
	}
	catch (const FileExistsException&) {
		throw BranchExistsException(name);
	}
}

void move_branch(const std::string& oldname, const std::string& newname, int flags)
{
	if (!is_branch(oldname))
		throw Exception(boost::format("branch doesn't exist: %1%") % oldname);

	if (is_branch(newname))
	{
		if (flags & Filesystem::FILE_FLAG_OVERWRITE)
			delete_branch(newname);
		else
			throw BranchExistsException(newname);
	}

	if (read_head() == oldname)
		write_head(newname);

	fs::rename(Globals::BranchDir / oldname, Globals::BranchDir / newname);
}

void copy_branch(const std::string& oldname, const std::string& newname, int flags)
{
	if (!is_branch(oldname))
		throw Exception(boost::format("branch doesn't exist: %1%") % oldname);

	if (is_branch(newname))
	{
		if (flags & Filesystem::FILE_FLAG_OVERWRITE)
			delete_branch(newname);
		else
			throw BranchExistsException(newname);
	}

	fs::copy(Globals::BranchDir / oldname, Globals::BranchDir / newname);
}

void delete_branch(const std::string& name, int flags)
{
	if (read_head() == name)
		throw Exception(boost::format("cannot delete branch '%1%', checked out") % name);
	if (!fs::remove(Globals::BranchDir / name))
		throw Exception(boost::format("branch doesn't exist: %1%") % name);
}

bool is_branch(const std::string& name)
{
	return fs::exists(Globals::BranchDir / name);
}

void write_head(const std::string& name)
{
	Filesystem::write_content(Globals::HeadFile, name, Filesystem::FILE_FLAG_OVERWRITE);
}

std::string read_head()
{
	return Filesystem::read_content(Globals::HeadFile);
}

void list_branches()
{
	if (!fs::exists(Globals::BranchDir))
		return;

	for (const fs::path branch : fs::directory_iterator(Globals::BranchDir))
	{
		std::cout << branch.filename().string() << '\n';
	}
}
