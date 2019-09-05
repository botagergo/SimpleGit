#pragma once

#include <sstream>

class ObjectFileWriter
{
	template <typename T>
	friend ObjectFileWriter& operator<< (ObjectFileWriter& writer, T val);
	template <typename T>
	friend ObjectFileWriter& operator<< (ObjectFileWriter&& writer, T val);
	friend ObjectFileWriter& operator<<(ObjectFileWriter& os, std::ostream& (*pf)(std::ostream&));

public:
	~ObjectFileWriter() { save(); }
	std::string save();
	std::string id() const;

private:
	std::stringstream	_stream;
	std::string		_id;
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

ObjectFileWriter& operator<<(ObjectFileWriter& writer, std::ostream& (*pf)(std::ostream&));