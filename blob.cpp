#include <fstream>
#include <windows.h>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object_file_writer.h"

std::wstring create_blob(const std::wstring &content)
{
	return (ObjectFileWriter() << L"blob" << L"\n" << content).save();
}

std::wstring create_blob_from_file(const std::filesystem::path &file)
{
	return create_blob(Filesystem::read_content(file));
}