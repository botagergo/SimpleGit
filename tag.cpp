#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "ref.h"
#include "tag.h"

class Object;

void create_tag(const std::wstring &tag_name, const std::wstring &ref, int flags = 0)
{
	fs::path path = Globals::TagDir / tag_name;
	try {
		Filesystem::write_content(path, ref, flags | Filesystem::FILE_FLAG_READONLY);
	}
	catch (FileExistsException) {
		throw TagExistsException(tag_name);
	}
}

void delete_tag(const std::wstring& tag_name)
{
	if (!fs::remove(Globals::TagDir / tag_name))
		throw TagDoesNotExistException(tag_name);
}
