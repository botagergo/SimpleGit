#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "helper.h"
#include "object.h"
#include "tree.h"

ObjectHeader parse_object_header(std::wistream& stream)
{
	ObjectHeader header;
	stream >> header.object_kind >> std::ws;
	return header;
}
