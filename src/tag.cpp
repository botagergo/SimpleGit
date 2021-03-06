#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "ref.h"
#include "tag.h"

class Object;

bool is_tag(const std::string& name)
{
	return fs::exists(Globals::TagDir / name);
}

std::string read_tag(const std::string& name)
{
	return Filesystem::read_content(Globals::TagDir / name);
}

void write_tag(const std::string &tag_name, const std::string& commit, int flags = 0)
{
	fs::path path = Globals::TagDir / tag_name;
	Filesystem::write_content(path, commit, flags | Filesystem::FILE_FLAG_CREATE_DIRECTORY);
}

void delete_tag(const std::string& tag_name)
{
	if (!fs::remove(Globals::TagDir / tag_name))
		throw TagDoesNotExistException(tag_name);
}

void list_tags()
{
	if (!fs::exists(Globals::TagDir))
		return;

	for (const fs::path tag : fs::directory_iterator(Globals::TagDir))
	{
		std::cout << tag.filename().string() << '\n';
	}
}
