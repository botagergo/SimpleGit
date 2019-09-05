#include <fstream>
#include <filesystem>

#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "tree.h"
#include "zlib/zlib.h"

namespace fs = boost::filesystem;

bool match_object_id(const std::string& prefix, const std::string& object_id)
{
	return std::equal(prefix.begin(), prefix.end(), object_id.begin());
}

std::string expand_object_id_prefix(const std::string& object_id_prefix)
{
	const std::string dir_prefix = object_id_prefix.substr(0, Globals::IdPrefixLength);
	const fs::path obj_dir = Filesystem::get_object_dir(dir_prefix);

	for (const fs::path& object : fs::directory_iterator(obj_dir))
	{
		if (match_object_id(object_id_prefix.substr(Globals::IdPrefixLength), object.filename().string()))
			return dir_prefix + object.filename().string();
	}

	throw InvalidObjectIdPrefix(object_id_prefix);
}

std::string open_object(std::ifstream& in_stream, const std::string& object_id)
{
	Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

	std::string object_type;
	in_stream >> object_type >> std::ws;

	return object_type;
}

std::string get_object_type(const std::string& object_id)
{
	Object object(object_id);
	ObjectHeader header;
	object.in_stream() >> header;
	return header.kind;
}

void pretty_print_object(std::ostream& out_stream, const std::string& object_id)
{
	Object object(object_id);
	ObjectStream stream = object.in_stream();
	ObjectHeader header;
	stream >> header;

	const std::string object_type = header.kind;
	if (object_type == "blob")
		out_stream << Filesystem::read_content(stream);
	else if (object_type == "tree")
		pretty_print_tree(out_stream, stream);
	else if (object_type == "commit")
		pretty_print_commit(out_stream, stream);
}

ObjectStream Object::in_stream()
{
	return ObjectStream(_path);;
}

ObjectStream & ObjectStream::operator>>(ObjectHeader & header)
{
	header = _header;
	return *this;
}
