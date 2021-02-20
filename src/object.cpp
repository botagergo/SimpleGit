#include "inttypes.h"
#include <fstream>
#include <sstream>

#include "blob.h"
#include "commit.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "tree.h"
#include <zlib.h>

namespace fs = boost::filesystem;

uint64_t create_file_map_read(const char* object_file, void** ptr, std::function<void(void)>& free_map);
void create_file_map_write(const char* object_file, void** ptr, uint64_t fsize, std::function<void(void)>& free_map);

ObjectData open_object(const char* object_file, bool header_only)
{
	ObjectData data;
	uLongf destLen;
	Bytef header_buf[ObjectHeader::MaxHeaderSize];

	Bytef* ptr;
	std::function<void(void)> free_map;
	uint64_t fsize = create_file_map_read(object_file, (void**)&ptr, free_map);

	z_stream stream;

	memset(&stream, Z_NULL, sizeof(z_stream));
	stream.avail_in = (uInt)fsize;
	stream.avail_out = ObjectHeader::MaxHeaderSize;
	stream.next_in = ptr;
	stream.next_out = header_buf;
	inflateInit(&stream);
	inflate(&stream, Z_NO_FLUSH);

	int n = sscanf((char*)header_buf, "%s %" PRId64 "\0", data.kind, &data.len);
	assert(n == 2);

	if (header_only)
	{
		data.buf = nullptr;
		goto ret;
	}

	data.buf = new char[ObjectHeader::MaxHeaderSize + data.len + 1];
	memset(data.buf, 0, ObjectHeader::MaxHeaderSize + data.len + 1);
	destLen = (uInt)(ObjectHeader::MaxHeaderSize + data.len);
	uncompress((Bytef*)data.buf, &destLen, ptr, (uLong)fsize);
	data.buf[destLen] = 0;

ret:
	free_map();
	return data;
}

void write_object(const char* object_file, const char* buf, uint64_t len)
{
	const uint64_t size = len * 2; //TODO change this

	Bytef* ptr;
	std::function<void(void)> free_map;
	create_file_map_write(object_file, (void**)&ptr, size, free_map);

	uLongf ulen = 1000;
	compress(ptr, &ulen, (Bytef*)buf, (uLong)size);

	free_map();
}

std::string ObjectWriter::save()
{
	if (_saved)
		return _id;

	std::stringstream header;
	header << _object_kind << ' ' << _buf.content_length() << '\0';

	char* buf_start = _buf.str() + Globals::MaxObjectHeaderSize - header.str().size();
	strcpy(buf_start, header.str().c_str());
	write_object(Globals::ObjectTmpFile.c_str(), buf_start, _buf.content_length() + header.str().size());

	_id = get_hash(buf_start, _buf.content_length() + header.str().size());

	// we check if the object already exists, it will throw exception when we try to overwrite otherwise
	if (Object(_id).exists())
		return _id;

	Filesystem::create_directory(Filesystem::get_object_dir(_id), Filesystem::FILE_FLAG_RECURSIVE);
	std::string object_file = Filesystem::get_object_path(_id).string();
	fs::copy(Globals::ObjectTmpFile, object_file);
	Filesystem::set_hidden(object_file.c_str());

	_saved = true;
	return _id;
}

std::string ObjectWriter::id() const
{
	return _id;
}

std::string Object::kind() const
{
	if (!_data.buf)
		_data = open_object(_path.string().c_str(), true);

	return _data.kind;
}

uint64_t Object::size() const
{
	if (!_data.buf)
		_data = open_object(_path.string().c_str(), true);

	return _data.len;
}

std::unique_ptr<ObjectReader> Object::get_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "blob") == 0)
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new BlobReader(data, *this)));
	else if (strcmp(data.kind, "tree") == 0)
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new TreeReader(data, *this)));
	else if (strcmp(data.kind, "commit") == 0)
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new CommitReader(data, *this)));
	else
		throw Exception(boost::format("invalid object kind: %1%") % data.kind);
}

std::unique_ptr<BlobReader> Object::get_blob_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "blob"))
		throw Exception(boost::format("object not a blob: ") % _id);
	return std::unique_ptr<BlobReader>((BlobReader*)(new BlobReader(data, *this)));
}

std::unique_ptr<TreeReader> Object::get_tree_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "tree"))
		throw Exception(boost::format("object not a tree: ") % _id);
	return std::unique_ptr<TreeReader>((TreeReader*)(new TreeReader(data, *this)));
}

std::unique_ptr<CommitReader> Object::get_commit_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "commit"))
		throw Exception(boost::format("object not a commit: ") % _id);
	return std::unique_ptr<CommitReader>((CommitReader*)(new CommitReader(data, *this)));
}

std::ostream& ObjectReader::read_content(std::ostream& out_stream)
{
	return out_stream << _curr_pos;
}
