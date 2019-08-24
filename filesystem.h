#pragma once

#include <windows.h>

namespace Filesystem
{
	enum FileFlags
	{
		FILE_FLAG_READONLY = FILE_ATTRIBUTE_READONLY,
		FILE_FLAG_HIDDEN = FILE_ATTRIBUTE_HIDDEN,
		FILE_FLAG_OVERWRITE = 4,
	};
	void open(const std::filesystem::path& path, std::wfstream &stream, int mode, bool create_directory = false);
	void create_directory(const std::filesystem::path& dir, bool hidden = false);
	void create_directory_recursive(const std::filesystem::path& dir, bool hidden);
	std::wstring read_content(const std::filesystem::path& file);
	std::wstring read_content(std::wistream& stream);
	void write_content(const std::filesystem::path &file, const std::wstring &content, int flags = 0);
	void write_content(const std::filesystem::path& file, std::wistream& in_stream, int flags = 0);

	std::filesystem::path get_object_path(const std::wstring& id);
	std::filesystem::path get_object_dir(const std::wstring& id);
}