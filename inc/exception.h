#pragma once

#include <exception>
#include <string>

#include <boost/format.hpp>

#include "defs.h"
#include "globals.h"
#include "helper.h"

class Exception : public std::exception
{
public:
	Exception(const std::string& msg) : _msg(msg) {}
	Exception(const boost::format& fmt) : Exception(fmt.str()) {}

	virtual const char* what() const noexcept override {
		return _msg.c_str();
	}

protected:
	const std::string  _msg;
};

class NotImplementedException : public Exception
{
public:
	NotImplementedException(const std::string &feature) : Exception(boost::format("feature not implemented: %1") % feature) {}
};

class ObjectNotFoundException : public Exception
{
public:
	ObjectNotFoundException(const std::string &id) : Exception(boost::format("object not found:  %1") % id), _id(id) {}

private:
	std::string		_id;
};

class ObjectFileEmptyException : public Exception
{
public:
	ObjectFileEmptyException(const fs::path &path) : Exception((boost::format("object file is empty: %1%") % path.string())) {}
};

class InvalidObjectTypeException : public Exception
{
public:
	InvalidObjectTypeException(const std::string &id, const std::string &type) : Exception(boost::format("invalid object type: %1%") % type), _id(id), _type(type) {}

private:
	std::string _id;
	std::string _type;
};

class TagExistsException : public Exception
{
public:
	TagExistsException(const std::string &tag_name) : Exception(boost::format("tag already exists: %1%") % tag_name), _tag_name(tag_name) {}

private:
	std::string _tag_name;
};

class TagDoesNotExistException : public Exception
{
public:
	TagDoesNotExistException(const std::string& tag_name) : Exception(boost::format("tag does not exist: %1%") % tag_name), _tag_name(tag_name) {}

private:
	std::string _tag_name;
};

class InvalidNameException : public Exception
{
public:
	InvalidNameException(const std::string& name) : Exception(boost::format("invalid reference: %1%") % name), _name(_name) {}

private:
	std::string _name;
};

class InvalidReferenceException : public Exception
{
public:
	InvalidReferenceException(const std::string& ref) : Exception(boost::format("invalid reference: %1%") % ref), _ref(ref) {}

private:
	std::string _ref;
};

class NotBlobException : public Exception
{
public:
	NotBlobException(const std::string& id) : Exception(boost::format("object is not a blob: %1%") % id), _id(id) {}

private:
	std::string _id;
};

class IndexFileNotFoundException : public Exception
{
public:
	IndexFileNotFoundException() : Exception(boost::format("index file not found: %1%") % Globals::IndexFile.generic_string()) {}
};

class FileNotInIndexException : public Exception
{
public:
	FileNotInIndexException(const fs::path& path) : Exception(boost::format("file not in index: %1%") % path.string()) {}
};

class FileAlreadyInIndexException : public Exception
{
public:
	FileAlreadyInIndexException(const fs::path& path) : Exception(boost::format("file already in index: %1%") % path.string()) {}
};

class BranchExistsException : public Exception
{
public:
	BranchExistsException(const std::string& branch_name) : Exception(boost::format("branch already exists: %1%") % branch_name) {}
};

class FileExistsException : public Exception
{
public:
	FileExistsException(const fs::path& path) : Exception(boost::format("file already exists: %1%") % path.string()) {}
};

class IndexRemoveFileExists : public Exception
{
public:
	IndexRemoveFileExists(const fs::path& path) : Exception(boost::format("file exists: %1%") % path.string()) {}
};

class FileOpenException : public Exception
{
public:
	FileOpenException(const fs::path& file) : Exception(boost::format("cannot open file: %1%") % file.string()), _file(file) {}
	const fs::path& file() const { return _file; }

private:
	fs::path _file;
};

class InvalidConfigFileFormatException : public Exception
{
public:
	InvalidConfigFileFormatException(const fs::path& config_file)
		: Exception(boost::format("invalid config file format: %1%") % config_file.string()), _config_file(config_file) {}
	const fs::path& file() const { return _config_file; }

private:
	fs::path _config_file;
};

class UserNameNotFoundException : public Exception
{
public:
	UserNameNotFoundException() : Exception(boost::format("user name not defined, define it using \"config\"")) {}
};

class IndexFileEmptyException : public Exception
{
public:
	IndexFileEmptyException() : Exception(boost::format("index file is empty")) {}
};

class NotCommitException : public Exception
{
public:
	NotCommitException(const std::string& object_id) : Exception(boost::format("object is not a commit: %1%") % object_id) {}

private:
	std::string object_id;
};

class NotTreeException : public Exception
{
public:
	NotTreeException(const std::string& object_id) : Exception(boost::format("object is not a tree: %1%") % object_id) {}

private:
	std::string object_id;
};


class CommitFileCorrupted : public Exception
{
public:
	CommitFileCorrupted(const fs::path& file) : Exception(boost::format("commit file corrupted: %1%") % file.string()) {}

private:
	fs::path file;
};

class TreeFileCorrupted : public Exception
{
public:
	TreeFileCorrupted(const fs::path& file) : Exception(boost::format("tree file corrupted: %1%") % file.string()) {}

private:
	fs::path file;
};

class InvalidObjectIdPrefix : public Exception
{
public:
	InvalidObjectIdPrefix(const fs::path& file) : Exception(boost::format("invalid object id prefix: %1%") % prefix), prefix(prefix) {}

private:
	std::string prefix;
};

class ResolveObjectException : public Exception
{
public:
	ResolveObjectException(const std::string& object_name) : Exception(boost::format("unable to resolve object name: %1%") % object_name), object_name(object_name) {}

private:
	std::string object_name;
};

class ResolveRefException : public Exception
{
public:
	ResolveRefException(const std::string& name) : Exception(boost::format("failed to resolve '%1%' as a valid ref") % name), _name(name) {}

private:
	std::string _name;
};

class MergeConflictException : public Exception
{
public:
	MergeConflictException() : Exception(boost::format("merge conflict")) {}
};

class PrefixNotFoundException : public Exception
{
public:
	PrefixNotFoundException(const fs::path& prefix) : Exception(boost::format("prefix not found: %1%") % prefix.string()), _prefix(prefix) {}

private:
	fs::path _prefix;
};

class HeadDoesntExistException : public Exception
{
public:
	HeadDoesntExistException() : Exception("head doesn't exist") {}
};
