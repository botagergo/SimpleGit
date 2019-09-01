#pragma once

#include <exception>
#include <filesystem>
#include <string>

#include <boost/format.hpp>

#include "globals.h"
#include "helper.h"

class Exception : public std::exception
{
public:
	Exception(const std::wstring& msg) : std::exception(to_string(msg).c_str()) {}
	Exception(const std::string& msg) : std::exception(msg.c_str()) {}
	Exception(const boost::wformat& fmt) : Exception(fmt.str()) {}
	Exception(const boost::format& fmt) : Exception(fmt.str()) {}
};

class NotImplementedException : public Exception
{
public:
	NotImplementedException(const std::wstring &feature) : Exception(boost::wformat(L"feature not implemented: %1") % feature) {}
};

class ObjectNotFoundException : public Exception
{
public:
	ObjectNotFoundException(const std::wstring &id) : Exception(boost::wformat(L"object not found:  %1") % id), _id(id) {}

private:
	std::wstring		_id;
};

class ObjectFileEmptyException : public Exception
{
public:
	ObjectFileEmptyException(const fs::path &path) : Exception((boost::wformat(L"object file is empty: %1%") % path.wstring())) {}
};

class InvalidObjectTypeException : public Exception
{
public:
	InvalidObjectTypeException(const std::wstring &id, const std::wstring &type) : Exception(boost::wformat(L"invalid object type: %1%") % type), _id(id), _type(type) {}

private:
	std::wstring _id;
	std::wstring _type;
};

class TagExistsException : public Exception
{
public:
	TagExistsException(const std::wstring &tag_name) : Exception(boost::wformat(L"tag already exists: %1%") % tag_name), _tag_name(tag_name) {}

private:
	std::wstring _tag_name;
};

class TagDoesNotExistException : public Exception
{
public:
	TagDoesNotExistException(const std::wstring& tag_name) : Exception(boost::wformat(L"tag does not exist: %1%") % tag_name), _tag_name(tag_name) {}

private:
	std::wstring _tag_name;
};

class InvalidNameException : public Exception
{
public:
	InvalidNameException(const std::wstring& name) : Exception(boost::wformat(L"invalid reference: %1%") % name), _name(_name) {}

private:
	std::wstring _name;
};

class InvalidReferenceException : public Exception
{
public:
	InvalidReferenceException(const std::wstring& ref) : Exception(boost::wformat(L"invalid reference: %1%") % ref), _ref(ref) {}

private:
	std::wstring _ref;
};

class NotBlobException : public Exception
{
public:
	NotBlobException(const std::wstring& id) : Exception(boost::wformat(L"object is not a blob: %1%") % id), _id(id) {}

private:
	std::wstring _id;
};

class IndexFileNotFoundException : public Exception
{
public:
	IndexFileNotFoundException() : Exception(boost::wformat(L"index file not found: %1%") % Globals::IndexFile.generic_wstring()) {}
};

class FileNotInIndexException : public Exception
{
public:
	FileNotInIndexException(const fs::path& path) : Exception(boost::wformat(L"file not in index: %1%") % path.wstring()) {}
};

class FileAlreadyInIndexException : public Exception
{
public:
	FileAlreadyInIndexException(const fs::path& path) : Exception(boost::wformat(L"file already in index: %1%") % path.wstring()) {}
};

class BranchExistsException : public Exception
{
public:
	BranchExistsException(const std::wstring& branch_name) : Exception(boost::wformat(L"branch already exists: %1%") % branch_name) {}
};

class FileExistsException : public Exception
{
public:
	FileExistsException(const fs::path& path) : Exception(boost::wformat(L"file already exists: %1%") % path.wstring()) {}
};

class IndexRemoveFileExists : public Exception
{
public:
	IndexRemoveFileExists(const fs::path& path) : Exception(boost::wformat(L"file exists: %1%") % path.wstring()) {}
};

class FileOpenException : public Exception
{
public:
	FileOpenException(const fs::path& file) : Exception(boost::wformat(L"cannot open file: %1%") % file.wstring()), _file(file) {}
	const fs::path& file() const { return _file; }

private:
	fs::path _file;
};

class InvalidConfigFileFormatException : public Exception
{
public:
	InvalidConfigFileFormatException(const fs::path& config_file)
		: Exception(boost::wformat(L"invalid config file format: %1%") % config_file.wstring()), _config_file(config_file) {}
	const fs::path& file() const { return _config_file; }

private:
	fs::path _config_file;
};

class UserNameNotFoundException : public Exception
{
public:
	UserNameNotFoundException() : Exception(boost::wformat(L"user name not defined, define it using \"config\"")) {}
};

class IndexFileEmptyException : public Exception
{
public:
	IndexFileEmptyException() : Exception(boost::wformat(L"index file is empty")) {}
};

class NotCommitException : public Exception
{
public:
	NotCommitException(const std::wstring& object_id) : Exception(boost::wformat(L"object is not a commit: %1%") % object_id) {}

private:
	std::wstring object_id;
};

class NotTreeException : public Exception
{
public:
	NotTreeException(const std::wstring& object_id) : Exception(boost::wformat(L"object is not a tree: %1%") % object_id) {}

private:
	std::wstring object_id;
};


class CommitFileCorrupted : public Exception
{
public:
	CommitFileCorrupted(const fs::path& file) : Exception(boost::wformat(L"commit file corrupted: %1%") % file.wstring()) {}

private:
	fs::path file;
};

class TreeFileCorrupted : public Exception
{
public:
	TreeFileCorrupted(const fs::path& file) : Exception(boost::wformat(L"tree file corrupted: %1%") % file.wstring()) {}

private:
	fs::path file;
};

class InvalidObjectIdPrefix : public Exception
{
public:
	InvalidObjectIdPrefix(const fs::path& file) : Exception(boost::wformat(L"invalid object id prefix: %1%") % prefix), prefix(prefix) {}

private:
	std::wstring prefix;
};

class ResolveObjectException : public Exception
{
public:
	ResolveObjectException(const std::wstring& object_name) : Exception(boost::wformat(L"unable to resolve object name: %1%") % object_name), object_name(object_name) {}

private:
	std::wstring object_name;
};

class ResolveRefException : public Exception
{
public:
	ResolveRefException(const std::wstring& name) : Exception(boost::wformat(L"failed to resolve '%1%' as a valid ref") % name), _name(name) {}

private:
	std::wstring _name;
};

class MergeConflictException : public Exception
{
public:
	MergeConflictException() : Exception(boost::wformat(L"merge conflict")) {}
};

class PrefixNotFoundException : public Exception
{
public:
	PrefixNotFoundException(const fs::path& prefix) : Exception(boost::wformat(L"prefix not found: %1%") % prefix.wstring()), _prefix(prefix) {}

private:
	fs::path _prefix;
};

class HeadDoesntExistException : public Exception
{
public:
	HeadDoesntExistException() : Exception(L"head doesn't exist") {}
};
