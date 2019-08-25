#include <filesystem>
#include <fstream>
#include <windows.h>

#include "exception.h"
#include "filesystem.h"

namespace Filesystem
{
	void open(const fs::path &path, std::wfstream &stream, int mode, bool create_directory)
	{
		if (create_directory)
		{
			fs::path dir = path;
			dir.remove_filename();
			create_directory_recursive(dir, false);
		}

		stream.open(path, mode);

		if (!stream)
			throw FileOpenException(path);
	}

	void create_directory(const fs::path &dir, bool hidden)
	{
		//TODO	use boost
		CreateDirectory(dir.c_str(), NULL);
		if (hidden)
			SetFileAttributes(dir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void create_directory_recursive(const fs::path &dir, bool hidden)
	{
		fs::path path;
		for (const fs::path &comp : dir)
			create_directory(path /= comp, hidden);
	}

	std::wstring read_content(const fs::path &file) // todo: throw exception
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

	void write_content(const fs::path &file, const std::wstring &content, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw FileExistsException(file);

		std::wfstream stream;
		open(file, (std::wfstream&)stream, std::ios_base::out);

		if (!stream)
			throw FileOpenException(file);

		stream << content;
		stream.close();

		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	void write_content(const fs::path& file, std::wistream& in_stream, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw FileExistsException(file);

		std::wfstream out_stream;

		open(file, out_stream, std::ios_base::out);
		write_content(out_stream, in_stream);
		out_stream.close();

		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	void write_content(std::wostream &out_stream, std::wistream &in_stream)
	{
		std::wstring line;
		while (std::getline(in_stream, line))
			out_stream << line << L"\n";
	}

	fs::path get_object_path(const std::wstring& id)
	{
		fs::path dir = get_object_dir(id);
		fs::path filename = id.substr(Globals::IdPrefixLength);
		return dir / filename;
	}

	fs::path get_object_dir(const std::wstring& id)
	{
		fs::path prefix = id.substr(0, Globals::IdPrefixLength);
		return Globals::ObjectDir / prefix;
	}

	std::wstring open_object(const std::wstring& object_id, std::wfstream& in_stream)
	{
		Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

		std::wstring object_type;
		in_stream >> object_type >> std::ws;

		return object_type;
	}
}