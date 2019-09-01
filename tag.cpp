#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "ref.h"
#include "tag.h"

class Object;

bool is_tag(const std::wstring& name)
{
	return fs::exists(Globals::TagDir / name);
}

std::wstring read_tag(const std::wstring& name)
{
	return Filesystem::read_content(Globals::TagDir / name);
}

void write_tag(const std::wstring &tag_name, const std::wstring& commit, int flags = 0)
{
	fs::path path = Globals::TagDir / tag_name;
	try {
		Filesystem::write_content(path, commit, flags | Filesystem::FILE_FLAG_CREATE_DIRECTORIES);
	}
	catch (const FileExistsException&) {
		throw TagExistsException(tag_name);
	}
}

void delete_tag(const std::wstring& tag_name)
{
	if (!fs::remove(Globals::TagDir / tag_name))
		throw TagDoesNotExistException(tag_name);
}

std::wstring resolve_tag(const std::wstring& tag_name)
{
	return expand_object_id_prefix(read_tag(tag_name));
}

void list_tags()
{
	if (!fs::exists(Globals::TagDir))
		return;

	for (const fs::path tag : fs::directory_iterator(Globals::TagDir))
	{
		message(tag.filename().wstring());
	}
}
