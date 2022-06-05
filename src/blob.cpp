#include <fstream>
#include <iostream>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "repository.h"

std::ostream& BlobReader::pretty_print(std::ostream& out_stream)
{
	out_stream << _curr_pos;
	return out_stream;
}

std::vector<std::string> BlobReader::read_lines()
{
	std::vector<std::string> lines;
	std::istringstream ss(_curr_pos);
	return Filesystem::read_lines(ss);
}

void BlobReader::operator>>(std::string& str)
{
	str = _curr_pos;
}

std::string write_blob(const Repository& repo, const std::string &content)
{
	return (ObjectWriter(repo, "blob") << content).save();
}

void read_blob(const Repository& repo, const std::string& blob_id, const fs::path& file)
{
	std::ofstream out_stream;
	Filesystem::open(file, out_stream, Filesystem::FILE_FLAG_CREATE_DIRECTORY);

	Object blob(repo, blob_id);
	blob.get_blob_reader()->read_content(out_stream);
}

std::string write_blob_from_file(const Repository& repo, const fs::path &file)
{
	return write_blob(repo, Filesystem::read_content(file));
}
