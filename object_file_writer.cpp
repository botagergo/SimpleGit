#include <filesystem>
#include <fstream>

#include "filesystem.h"
#include "helper.h"
#include "object_file_writer.h"

std::wstring ObjectFileWriter::save()
{
	if (_saved)
		return _id;

	_id = get_hash(_stream.str());

	// we check if the object already exists, it will throw exception when we try to overwrite otherwise
	if (Filesystem::object_exists(_id))
		return _id;

	std::wofstream out_stream;
	Filesystem::open(Filesystem::get_object_path(_id), out_stream, true);
	out_stream << _stream.str();
	out_stream.close();

	SetFileAttributes(Filesystem::get_object_path(_id).c_str(), FILE_ATTRIBUTE_READONLY);
	_saved = true;

	return _id;
}

std::wstring ObjectFileWriter::id() const
{
	return _id;
}

ObjectFileWriter& operator<<(ObjectFileWriter& writer, std::wostream& (*pf)(std::wostream&))
{
	std::wstring str;
	writer._stream << str;
	return writer;
}
