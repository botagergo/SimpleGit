#pragma once

#include <windows.h>

namespace fs = std::filesystem;

namespace Filesystem
{
	enum FileFlags
	{
		FILE_FLAG_READONLY = FILE_ATTRIBUTE_READONLY,
		FILE_FLAG_HIDDEN = FILE_ATTRIBUTE_HIDDEN,
		FILE_FLAG_OVERWRITE = 4,
	};

	void open(const fs::path& path, std::wfstream &stream, int mode, bool create_directory = false);
	void create_directory(const fs::path& dir, bool hidden = false);
	void create_directory_recursive(const fs::path& dir, bool hidden);

	std::wstring read_content(const fs::path& file);
	std::wstring read_content(std::wistream& stream);

	void write_content(const fs::path &file, const std::wstring &content, int flags = 0);
	void write_content(const fs::path& file, std::wistream& in_stream, int flags = 0);
	void write_content(std::wostream& out_stream, std::wistream& in_stream);

	fs::path get_object_path(const std::wstring& id);
	fs::path get_object_dir(const std::wstring& id);

	std::wstring open_object(const std::wstring& object_id, std::wfstream& in_stream);
}
