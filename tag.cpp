#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "ref.h"
#include "tag.h"

class Object;

void create_tag(const std::wstring &tag_name, const std::wstring &ref, int flags = 0)
{
	std::filesystem::path path = Config::TagDir / tag_name;
	try {
		Filesystem::write_content(path, ref, flags | Filesystem::FILE_FLAG_READONLY);
	}
	catch (FileExistsException) {
		throw TagExistsException(tag_name);
	}
}

void delete_tag(const std::wstring& tag_name)
{
	if (!std::filesystem::remove(Config::TagDir / tag_name))
		throw TagDoesNotExistException(tag_name);
}
