#pragma once

#include <sys/stat.h>
#include <windows.h>

#include <boost/filesystem.hpp>

#include "exception.h"

namespace fs = boost::filesystem;

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

		stream.open(path.wstring());

		if (!stream)
			throw FileOpenException(path);
	}

	void create_directory(const fs::path& dir, bool hidden = false);

	std::wstring read_content(const fs::path& file);
	std::wstring read_content(std::wistream& stream);

	void write_content(const fs::path &file, const std::wstring &content, int flags = 0);
	void write_content(const fs::path& file, std::wistream& in_stream, int flags = 0);
	void write_content(std::wostream& out_stream, std::wistream& in_stream);

	fs::path get_object_path(const std::wstring& id);
	fs::path get_object_dir(const std::wstring& id);

	std::wstring open_object(const std::wstring& object_id, std::wifstream& in_stream);

	bool object_exists(const std::wstring& object_id);

	bool is_prefix(const fs::path& prefix, const fs::path& path);

	void make_sure_file_exists(const fs::path& file);

	struct stat get_stat(const fs::path& file);
}
