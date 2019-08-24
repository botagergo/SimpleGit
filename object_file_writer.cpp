#include <filesystem>
#include <fstream>

#include "filesystem.h"
#include "helper.h"
#include "object_file_writer.h"

std::wstring ObjectFileWriter::save()
{
	_id = get_hash(_stream.str());
	std::wfstream stream;
	Filesystem::open(Filesystem::get_object_dir(_id) / _id, stream, std::ios_base::out, true);
	stream << _stream.str();
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
