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
		FILE_FLAG_READONLY = FILE_ATTRIBUTE_READONLY,
		FILE_FLAG_HIDDEN = FILE_ATTRIBUTE_HIDDEN,
		FILE_FLAG_OVERWRITE = 4,
		FILE_FLAG_CREATE_DIRECTORIES = 8,
	};

	template <typename FileStream>
	void open(fs::path path, FileStream& stream, bool create_directory=false)
	{
		if (create_directory)
		{
			fs::path dir = path;
			dir.remove_filename();
			Filesystem::create_directory(dir, false);
		}

		stream.open(path.string());

		if (!stream)
			throw Exception(boost::format("cannot open file: %1%") % path);
	}

	void create_directory(const fs::path& dir, bool hidden = false);

	std::string read_content(const fs::path& file);
	std::string read_content(std::istream& stream);

	void write_content(const fs::path &file, const std::string &content, int flags = 0);
	void write_content(const fs::path& file, std::istream& in_stream, int flags = 0);
	void write_content(std::ostream& out_stream, std::istream& in_stream);

	fs::path get_object_path(const std::string& id);
	fs::path get_object_dir(const std::string& id);

	std::string open_object(const std::string& object_id, std::ifstream& in_stream);

	bool object_exists(const std::string& object_id);

	bool is_prefix(const fs::path& prefix, const fs::path& path);

	void make_sure_file_exists(const fs::path& file);

	struct stat get_stat(const fs::path& file);
}
