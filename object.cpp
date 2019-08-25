#include <fstream>

#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "tree.h"

std::wstring open_object(const std::wstring& object_id, std::wfstream& in_stream)
{
	std::wfstream in_stream;
	Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

	std::wstring object_type;
	in_stream >> object_type >> std::ws;

	return object_type;
}
