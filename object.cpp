#include <fstream>

#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "tree.h"

bool match_object_id(const std::wstring& prefix, const std::wstring& object_id)
{
	return std::equal(prefix.begin(), prefix.end(), object_id.begin());
}

std::wstring expand_object_id_prefix(const std::wstring& object_id_prefix)
{
	const std::wstring dir_prefix = object_id_prefix.substr(0, Globals::IdPrefixLength);
	const fs::path obj_dir = Filesystem::get_object_dir(dir_prefix);

	for (const fs::path& object : fs::directory_iterator(obj_dir))
	{
		if (match_object_id(object_id_prefix.substr(Globals::IdPrefixLength), object.filename().wstring()))
			return dir_prefix + object.filename().wstring();
	}

	throw InvalidObjectIdPrefix(object_id_prefix);
}

std::wstring open_object(std::wifstream& in_stream, const std::wstring& object_id)
{
	Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

	std::wstring object_type;
	in_stream >> object_type >> std::ws;

	return object_type;
}

std::wstring get_object_type(const std::wstring& object_id)
{
	std::wifstream in_stream;
	return open_object(in_stream, object_id);
}

void pretty_print_object(std::wostream& out_stream, const std::wstring& object_id)
{
	std::wifstream in_stream;
	const std::wstring object_type = open_object(in_stream, object_id);
	if (object_type == L"blob")
		out_stream << Filesystem::read_content(in_stream);
	else if (object_type == L"tree")
		pretty_print_tree(out_stream, in_stream);
	else if (object_type == L"commit")
		pretty_print_commit(out_stream, in_stream);
}
