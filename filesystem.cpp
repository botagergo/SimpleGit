#include <filesystem>
#include <fstream>
#include <windows.h>

#include "exception.h"
#include "filesystem.h"

namespace Filesystem
{
	void open(const std::filesystem::path &path, std::wfstream &stream, int mode, bool create_directory)
	{
		if (create_directory)
		{
			std::filesystem::path dir = path;
			dir.remove_filename();
			create_directory_recursive(dir, false);
		}

		stream.open(path, mode);

		if (!stream.is_open())
			throw FileOpenException(path);
	}

	void create_directory(const std::filesystem::path &dir, bool hidden)
	{
		//TODO	use boost
		CreateDirectory(dir.c_str(), NULL);
		if (hidden)
			SetFileAttributes(dir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void create_directory_recursive(const std::filesystem::path &dir, bool hidden)
	{
		std::filesystem::path path;
		for (const std::filesystem::path &comp : dir)
			create_directory(path /= comp, hidden);
	}

	std::wstring read_content(const std::filesystem::path &file) // todo: throw exception
	{
		std::wfstream stream;
		open(file, stream, std::ios_base::in);
		return read_content(stream);
	}

	std::wstring read_content(std::wistream& stream)
	{
		return std::wstring((std::istreambuf_iterator<wchar_t>(stream)),
			std::istreambuf_iterator<wchar_t>());
	}

	void write_content(const std::filesystem::path &file, const std::wstring &content, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && std::filesystem::exists(file))
			throw FileExistsException(file);

		std::wfstream stream;
		open(file, stream, std::ios_base::out);

		if (!stream)
			throw FileOpenException(file);

		stream << content;
		stream.close();

		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	void write_content(const std::filesystem::path& file, std::wistream& in_stream, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && std::filesystem::exists(file))
			throw FileExistsException(file);

		std::wfstream stream;
		open(file, stream, std::ios_base::out);

		std::wstring line;
		while (std::getline(in_stream, line))
			stream << line << L"\n";

		stream.close();
		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	std::filesystem::path get_object_path(const std::wstring& id)
	{
		std::filesystem::path dir = get_object_dir(id);
		std::filesystem::path filename = id.substr(Config::IdPrefixLength);
		return dir / filename;
	}

	std::filesystem::path get_object_dir(const std::wstring& id)
	{
		std::filesystem::path prefix = id.substr(0, Config::IdPrefixLength);
		return Config::ObjectDir / prefix;
	}
}