#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "ref.h"
#include "repository.h"
#include "tag.h"

class Object;

bool is_tag(const Repository& repo, const std::string& name)
{
	return fs::exists(repo.tagDir / name);
}

std::string read_tag(const Repository& repo, const std::string& name)
{
	return Filesystem::read_content(repo.tagDir / name);
}

void write_tag(const Repository& repo, const std::string &tag_name, const std::string& commit, int flags = 0)
{
	fs::path path = repo.tagDir / tag_name;
	Filesystem::write_content(path, commit, flags | Filesystem::FILE_FLAG_CREATE_DIRECTORY);
}

void delete_tag(const Repository& repo, const std::string& tag_name)
{
	if (!fs::remove(repo.tagDir / tag_name))
		throw TagDoesNotExistException(tag_name);
}

void list_tags(const Repository& repo)
{
	if (!fs::exists(repo.tagDir))
		return;

	for (const fs::path tag : fs::directory_iterator(repo.tagDir))
	{
		std::cout << tag.filename().string() << '\n';
	}
}
