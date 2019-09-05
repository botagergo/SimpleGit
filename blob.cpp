#include <fstream>
#include <windows.h>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object_file_writer.h"

std::string write_blob(const std::string &content)
{
	return (ObjectFileWriter() << "blob" << '\n' << content).save();
}

void read_blob(const std::string& id, const fs::path& file)
{
	std::ifstream in_stream;
	std::string object_type = Filesystem::open_object(id, in_stream);
	if (object_type != "blob")
		throw NotBlobException(id);

	std::ofstream out_stream;
	Filesystem::open(file, out_stream, true);
	Filesystem::write_content(out_stream, in_stream);
}

std::string write_blob_from_file(const fs::path &file)
{
	return write_blob(Filesystem::read_content(file));
}