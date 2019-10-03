#pragma once

#include <sys/stat.h>
#include <windows.h>

#include <boost/filesystem/path.hpp>

#include "defs.h"
#include "exception.h"

namespace Filesystem
{
	enum FileFlags
	{
		FILE_FLAG_READONLY			= 0,
		FILE_FLAG_HIDDEN			= 2,
		FILE_FLAG_OVERWRITE			= 4,
		FILE_FLAG_CREATE_DIRECTORY	= 8,
	};

	template <typename FileStream>
	void open(const fs::path& path, FileStream& stream, int flags = 0)
	{
		if (flags & FILE_FLAG_CREATE_DIRECTORY)
			Filesystem::create_directory(path.parent_path(), flags);

		stream.open(path.string());

		if (!stream)
			throw Exception(boost::format("cannot open file: %1%") % path);
	}

	void create_directory(const fs::path& dir, int flags = 0);

	std::string read_content(const fs::path& file);
	std::string read_content(std::istream& stream);

	std::vector<std::string> read_lines(std::istream& stream);
	std::vector<std::string> read_lines(const fs::path& file);

	void write_content(const fs::path &file, const std::string &content, int flags = 0);
	void write_content(const fs::path& file, std::istream& in_stream, int flags = 0);
	void write_content(std::ostream& out_stream, std::istream& in_stream);

	fs::path get_object_path(const std::string& id);
	fs::path get_object_dir(const std::string& id);

	bool is_prefix(const fs::path& prefix, const fs::path& path);

	void make_sure_file_exists(const fs::path& file);

	struct stat get_stat(const fs::path& file);
}
