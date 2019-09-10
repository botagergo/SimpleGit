#pragma once

#include <algorithm>
#include <iostream>

#include <boost/filesystem.hpp>

#include "defs.h"
#include "filesystem.h"
#include "globals.h"

class Object;
class BlobReader;
class TreeReader;
class CommitReader;

struct ObjectHeader
{
	const static long long MaxHeaderSize = 32;
	std::string kind;
	long long	size;
};

// contains data about an uncompressed object
struct ObjectData
{
	// type of the object
	char		kind[10];

	// null-terminated string, contains the entire uncompressed content or null,
	// depending on whether the header_only argument was true to the call of open_object
	char*		buf;

	// the length of the object, not counting the header
	uint64_t	len;
};

// base class for reading from an object
// accessible from the Object::get_reader methods
class ObjectReader
{
	friend class Object;

	ObjectReader& operator=(const ObjectReader& other) = delete;

public:
	virtual std::ostream&	pretty_print(std::ostream& out_stream) = 0;
	std::ostream&			read_content(std::ostream& out_stream);

	operator bool() const {
		return _valid;
	}

	~ObjectReader()
	{
		delete[] _data.buf;
	}

protected:
	ObjectReader(const ObjectData& data) : _data(data), _curr_pos(data.buf), _valid(true)
	{
		_curr_pos += strlen(_curr_pos) + 1;
		_end = _curr_pos + data.len;
	}

	ObjectData	_data;
	char*		_curr_pos;
	char*		_end;
	bool		_valid;
};

class ObjectWriterStreambuf : public std::streambuf
{
public:
	ObjectWriterStreambuf()
	{
		_buf = (char*)malloc(_start_len);
		setp(_buf + Globals::MaxObjectHeaderSize, _buf + _start_len - Globals::MaxObjectHeaderSize);
	}

	// returns the length of the buffer, not including the space reserved for the header
	uint64_t content_length() const
	{
		return pptr() - _buf - Globals::MaxObjectHeaderSize;
	}

	// returns a pointer to the beginning of the buffer, including the space reserved for the header
	char* str()
	{
		*pptr() = 0;
		return _buf;
	}

	~ObjectWriterStreambuf()
	{
		free(_buf);
	}

private:
	const static size_t _start_len = 2048;
	char* _buf;
};

class ObjectWriter
{
	template <typename T>
	friend ObjectWriter& operator<< (ObjectWriter&& writer, const T& val);
public:
	ObjectWriter(const std::string& object_kind) : _object_kind(object_kind), _stream(&_buf) {}

	~ObjectWriter() { save(); }
	std::string save();
	std::string id() const;

	template <typename T>
	ObjectWriter& operator<< (const T& val)
	{
		_stream << val;
		return *this;
	}

private:
	ObjectWriterStreambuf	_buf;
	std::ostream			_stream;
	std::string				_id;
	bool					_saved = false;
	std::string				_object_kind;
};

template <typename T>
ObjectWriter& operator<< (ObjectWriter&& writer, const T& val)
{
	writer._stream << val;
	return writer;
}

class Object
{
public:
	Object(const std::string& id) : _id(id), _path(Filesystem::get_object_path(id)) {}

	bool				exists() const { return fs::exists(_path); }
	const std::string&	id() const { return _id; }
	const fs::path&		path() const { return _path; }
	std::string			kind() const;
	uint64_t			size() const;

	std::unique_ptr<ObjectReader>	get_reader() const;
	std::unique_ptr<BlobReader>		get_blob_reader() const;
	std::unique_ptr<TreeReader>		get_tree_reader() const;
	std::unique_ptr<CommitReader>	get_commit_reader() const;

private:
	std::string			_id;
	fs::path			_path;
	mutable ObjectData	_data;
};