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
#include "zlib/zlib.h"

namespace fs = boost::filesystem;

ObjectData open_object(const char* object_file, bool header_only)
{
	ObjectData data;
	uLongf destLen;
	Bytef header_buf[ObjectHeader::MaxHeaderSize];

	HANDLE file = CreateFileA(object_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (file == INVALID_HANDLE_VALUE)
		throw Exception(boost::format("cannot open file: %1%") % object_file);

	HANDLE map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (map == NULL)
		throw Exception("CreateFileMapping");

	Bytef* ptr = (Bytef*)MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
	if (ptr == NULL)
		throw Exception("MapViewOfFile");

	LARGE_INTEGER l_fsize;
	GetFileSizeEx(file, (PLARGE_INTEGER)& l_fsize);
	uint64_t fsize = l_fsize.QuadPart;

	z_stream stream;

	memset(&stream, Z_NULL, sizeof(z_stream));
	stream.avail_in = (uInt)fsize;
	stream.avail_out = ObjectHeader::MaxHeaderSize;
	stream.next_in = ptr;
	stream.next_out = header_buf;
	inflateInit(&stream);
	inflate(&stream, Z_NO_FLUSH);

	int n = sscanf((char*)header_buf, "%s %lld\0", data.kind, &data.len);
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
	UnmapViewOfFile(ptr);
	CloseHandle(file);

	return data;
}

void write_object(const char* object_file, const char* buf, uint64_t len)
{
	HANDLE file = CreateFileA(object_file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	if (file == INVALID_HANDLE_VALUE)
		throw Exception(boost::format("cannot create file: %1%") % object_file);

	HANDLE map = CreateFileMappingA(file, NULL, PAGE_READWRITE, len >> (sizeof(DWORD) * 8), len & ((DWORD)-1), NULL);
	if (map == NULL)
		throw Exception("CreateFileMapping");

	Bytef* ptr = (Bytef*)MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, len & ((DWORD)-1));
	if (ptr == NULL)
		throw Exception("MapViewOfFile");

	uLongf ulen = len;
	compress(ptr, &ulen, (Bytef*)buf, len);
	UnmapViewOfFile(ptr);
	CloseHandle(file);
}

std::string ObjectWriter::save()
{
	if (_saved)
		return _id;

	std::stringstream header;
	header << _object_kind << ' ' << _buf.content_length() << '\0';

	char* buf_start = _buf.str() + Globals::MaxObjectHeaderSize - header.str().size();
	strcpy(buf_start, header.str().c_str());

	_id = get_hash(buf_start, _buf.content_length() + header.str().size());

	// we check if the object already exists, it will throw exception when we try to overwrite otherwise
	if (Filesystem::object_exists(_id))
		return _id;

	Filesystem::create_directory(Filesystem::get_object_dir(_id));

	std::string object_file = Filesystem::get_object_path(_id).string();

	write_object(object_file.c_str(), buf_start, _buf.content_length() + header.str().size());

	SetFileAttributesA(object_file.c_str(), FILE_ATTRIBUTE_READONLY);
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
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new BlobReader(data)));
	else if (strcmp(data.kind, "tree") == 0)
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new TreeReader(data)));
	else if (strcmp(data.kind, "commit") == 0)
		return std::unique_ptr<ObjectReader>((ObjectReader*)(new CommitReader(data)));
	else
		throw Exception(boost::format("invalid object kind: %1%") % data.kind);
}

std::unique_ptr<BlobReader> Object::get_blob_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "blob"))
		throw Exception(boost::format("object not a blob: ") % _id);
	return std::unique_ptr<BlobReader>((BlobReader*)(new BlobReader(data)));
}

std::unique_ptr<TreeReader> Object::get_tree_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "tree"))
		throw Exception(boost::format("object not a tree: ") % _id);
	return std::unique_ptr<TreeReader>((TreeReader*)(new TreeReader(data)));
}

std::unique_ptr<CommitReader> Object::get_commit_reader() const
{
	ObjectData data = open_object(_path.string().c_str(), false);
	if (strcmp(data.kind, "commit"))
		throw Exception(boost::format("object not a commit: ") % _id);
	return std::unique_ptr<CommitReader>((CommitReader*)(new CommitReader(data)));
}

std::ostream& ObjectReader::read_content(std::ostream& out_stream)
{
	return out_stream << _curr_pos;
}
