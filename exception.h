#pragma once

#include <exception>
#include <filesystem>
#include <string>

#include "globals.h"

class Exception
{
public:
	Exception(const std::wstring &msg) : _msg(msg) {}
	std::wstring message() const { return _msg; }

private:
	std::wstring _msg;
};

class NotImplementedException : public Exception
{
public:
	NotImplementedException(const std::wstring &feature) : Exception(std::wstring(L"feature not implemented: ") + feature) {}
};

class ObjectNotFoundException : public Exception
{
public:
	ObjectNotFoundException(const std::wstring &id) : Exception((std::wstring(L"object not found: ") + id)), _id(id) {}

private:
	std::wstring		_id;
};

class ObjectFileEmptyException : public Exception
{
public:
	ObjectFileEmptyException(const std::filesystem::path &path) : Exception((std::wstring(L"object file is empty: ") + path.wstring())) {}
};

class InvalidObjectTypeException : public Exception
{
public:
	InvalidObjectTypeException(const std::wstring &id, const std::wstring &type) : Exception(std::wstring(L"invalid object type: ") + type), _id(id), _type(type) {}

private:
	std::wstring _id;
	std::wstring _type;
};

class TagExistsException : public Exception
{
public:
	TagExistsException(const std::wstring &tag_name) : Exception(std::wstring(L"tag already exists: ") + tag_name), _tag_name(tag_name) {}

private:
	std::wstring _tag_name;
};

class TagDoesNotExistException : public Exception
{
public:
	TagDoesNotExistException(const std::wstring& tag_name) : Exception(std::wstring(L"tag does not exist: ") + tag_name), _tag_name(tag_name) {}

private:
	std::wstring _tag_name;
};

class InvalidNameException : public Exception
{
public:
	InvalidNameException(const std::wstring &name) : Exception(std::wstring(L"invalid reference: ") + name), _name(_name) {}

private:
	std::wstring _name;
};

class InvalidReferenceException : public Exception
{
public:
	InvalidReferenceException(const std::wstring &ref) : Exception(std::wstring(L"invalid reference: ") + ref), _ref(ref) {}

private:
	std::wstring _ref;
};

class NotBlobException : public Exception
{
public:
	NotBlobException(const std::wstring &id) : Exception(std::wstring(L"object is not a blob: ") + id), _id(id) {}

private:
	std::wstring _id;
};

class IndexFileNotFoundException : public Exception
{
public:
	IndexFileNotFoundException() : Exception(std::wstring(L"index file not found: ") + Config::IndexFile.generic_wstring()) {}
};

class FileNotInIndexException : public Exception
{
public:
	FileNotInIndexException(const std::filesystem::path &path) : Exception(std::wstring(L"file not in index: ") + path.wstring()) {}
};

class FileAlreadyInIndexException : public Exception
{
public:
	FileAlreadyInIndexException(const std::filesystem::path &path) : Exception(std::wstring(L"file already in index: ") + path.wstring()) {}
};

class BranchExistsException : public Exception
{
public:
	BranchExistsException(const std::wstring &branch_name) : Exception(std::wstring(L"branch already exists: ") + branch_name) {}
};

class FileExistsException : public Exception
{
public:
	FileExistsException(const std::filesystem::path &path) : Exception(std::wstring(L"file already exists: ") + path.wstring()) {}
};

class IndexRemoveFileExists : public Exception
{
public:
	IndexRemoveFileExists(const std::filesystem::path &path) : Exception(std::wstring(L"file exists: ") + path.wstring()) {}
};

class FileOpenException : public Exception
{
public:
	FileOpenException(const std::filesystem::path &file) : Exception(std::wstring(L"cannot open file: ") + file.wstring()), _file(file) {}
	const std::filesystem::path& file() const { return _file; }

private:
	std::filesystem::path _file;
};
