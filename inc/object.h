#pragma once

#include <algorithm>
#include <iostream>

#include <boost/filesystem.hpp>

#include "defs.h"

class Object;
class BlobReader;
class TreeReader;
class CommitReader;
class Repository;

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
	char*		buf = nullptr;

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

	const Object& object() const {
		return _object;
	}

	operator bool() const {
		return _valid;
	}

	~ObjectReader()
	{
		delete[] _data.buf;
	}

protected:
	ObjectReader(const ObjectData& data, const Object& object) : _data(data), _curr_pos(data.buf), _valid(true), _object(object)
	{
		_curr_pos += strlen(_curr_pos) + 1;
		_end = _curr_pos + data.len;
	}

	const Object&	_object;
	ObjectData		_data;
	char*			_curr_pos;
	char*			_end;
	bool			_valid;
};

class ObjectWriterStreambuf : public std::streambuf
{
public:
	ObjectWriterStreambuf();

	// returns the length of the buffer, not including the space reserved for the header
	uint64_t content_length() const;

	// returns a pointer to the beginning of the buffer, including the space reserved for the header
	char* str();

	~ObjectWriterStreambuf();

private:
	const static size_t _start_len = 2048;
	char* _buf;
};

class ObjectWriter
{
	template <typename T>
	friend ObjectWriter&& operator<< (ObjectWriter&& writer, const T& val);
	template <typename T>
	friend ObjectWriter& operator<< (ObjectWriter& writer, const T& val);
public:
	ObjectWriter(const Repository& repo, const std::string& object_kind)
		: _repo(repo), _object_kind(object_kind), _stream(&_buf) {}

	~ObjectWriter() { save(); }
	std::string save();
	std::string id() const;


private:
	ObjectWriterStreambuf	_buf;
	std::ostream			_stream;
	std::string				_id;
	bool					_saved = false;
	std::string				_object_kind;
	const Repository&		_repo;
};

template <typename T>
ObjectWriter& operator<< (ObjectWriter& writer, const T& val)
{
	writer._stream << val;
	return writer;
}

template <typename T>
ObjectWriter&& operator<< (ObjectWriter&& writer, const T& val)
{
	writer._stream << val;
	return std::move(writer);
}

fs::path			get_object_path(const Repository& repo, const std::string& id);
fs::path			get_object_dir(const Repository& repo, const std::string& id);


class Object
{
public:
	Object(const Repository& repo, const std::string& id) :
		_repo(repo), _id(id), _path(get_object_path(repo, id)) {}

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

	const Repository&   _repo;

	mutable ObjectData	_data;
};
