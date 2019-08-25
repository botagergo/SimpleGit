#include <fstream>
#include <windows.h>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object_file_writer.h"

std::wstring write_blob(const std::wstring &content)
{
	return (ObjectFileWriter() << L"blob" << L'\n' << content).save();
}

void read_blob(const std::wstring& id, const fs::path& file)
{
	std::wfstream in_stream;
	std::wstring object_type = Filesystem::open_object(id, in_stream);
	if (object_type != L"blob")
		throw NotBlobException(id);

	std::wfstream out_stream;
	Filesystem::open(file, out_stream, std::ios_base::out, true);
	Filesystem::write_content(out_stream, in_stream);
}

std::wstring write_blob_from_file(const fs::path &file)
{
	return write_blob(Filesystem::read_content(file));
}