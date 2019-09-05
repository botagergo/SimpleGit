#pragma once

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "filesystem.h"
#include "zlib/zlib.h"

namespace fs = boost::filesystem;

struct ObjectHeader
{
public:
	ObjectHeader(const std::string kind, size_t size) : kind(kind), size(size) {}
	ObjectHeader() {}

	std::string kind;
	size_t		size;
	const static size_t MaxHeaderSize = 32;
};

class ObjectStream
{
	friend class Object;
public:
	ObjectStream& operator>> (ObjectHeader& header);
	ObjectStream& operator<< (const ObjectHeader& header);

private:
	ObjectStream(const fs::path& file)
	{
		Filesystem::open(file, _in_stream);

		char header_buf[ObjectHeader::MaxHeaderSize];
		_in_stream.read(header_buf, sizeof(header_buf));
		char uncompressed_header_buf[ObjectHeader::MaxHeaderSize];
		uLongf uncompressed_header_buf_len;

		uncompress((Bytef*)uncompressed_header_buf, &uncompressed_header_buf_len, (Bytef*)header_buf, sizeof(header_buf));
		char object_kind[10];
		int object_size;
		sscanf(uncompressed_header_buf, "%s %d\0", object_kind, &object_size);
		_header.kind = object_kind;
		_header.size = object_size;

		std::string content = Filesystem::read_content(_in_stream);
		char *uncompressed_content_buf = new char[object_size];
		uLongf uncompressed_content_buf_size;

		uncompress((Bytef*)uncompressed_content_buf, &uncompressed_content_buf_size, (Bytef*)content.c_str(), content.size());
		_content_in_stream.str(uncompressed_content_buf);
	}

	ObjectHeader	_header;
	std::ifstream	_in_stream;
	std::istringstream _content_in_stream;
};

class Object
{
public:
	struct Header
	{
		std::string	kind;
		size_t			length;
	};

	Object(const std::string& id) : _object_id(id), _path(Filesystem::get_object_path(id)) {}

	bool					exists() const;
	const fs::path&			get_path() const { return _path; };

	ObjectStream		in_stream();
	ObjectStream		out_stream();

private:
	std::string		_object_id;
	fs::path		_path;
};

bool			match_object_id(const std::string& prefix, const std::string& object_id);

std::string	expand_object_id_prefix(const std::string& object_id_prefix);

std::string	open_object(std::ifstream& in_stream, const std::string& object_id);

std::string	get_object_type(const std::string& object_id);

void			pretty_print_object(std::ostream& out_stream, const std::string& object_id);