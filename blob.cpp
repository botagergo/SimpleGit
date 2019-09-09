#include <fstream>
#include <windows.h>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"

std::string write_blob(const std::string &content)
{
	return (ObjectWriter("blob") << content).save();
}

void read_blob(const std::string& blob_id, const fs::path& file)
{
	std::ofstream out_stream;
	Filesystem::open(file, out_stream, true);

	Object blob(blob_id);
	blob.get_blob_reader()->read_content(out_stream);
}

std::string write_blob_from_file(const fs::path &file)
{
	return write_blob(Filesystem::read_content(file));
}