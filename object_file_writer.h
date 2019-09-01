#pragma once

#include <sstream>

class ObjectFileWriter
{
	template <typename T>
	friend ObjectFileWriter& operator<< (ObjectFileWriter& writer, T val);
	template <typename T>
	friend ObjectFileWriter& operator<< (ObjectFileWriter&& writer, T val);
	friend ObjectFileWriter& operator<<(ObjectFileWriter& os, std::wostream& (*pf)(std::wostream&));

public:
	~ObjectFileWriter() { save(); }
	std::wstring save();
	std::wstring id() const;

private:
	std::wstringstream	_stream;
	std::wstring		_id;
	bool				_saved = false;
};

template <typename T>
ObjectFileWriter& operator<< (ObjectFileWriter& writer, T val)
{
	writer._stream << val;
	return writer;
}

template <typename T>
ObjectFileWriter& operator<< (ObjectFileWriter&& writer, T val)
{
	writer._stream << val;
	return writer;
}

ObjectFileWriter& operator<<(ObjectFileWriter& writer, std::wostream& (*pf)(std::wostream&));