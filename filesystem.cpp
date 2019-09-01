#include <fstream>
#include <windows.h>

#include "exception.h"
#include "filesystem.h"

namespace Filesystem
{
	void create_directory(const fs::path &dir, bool hidden)
	{
		boost::system::error_code ec;
		bool b = fs::create_directories(dir, ec);
		auto m = ec.message();
		if (hidden)
			SetFileAttributes(dir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	std::wstring read_content(const fs::path &file) // todo: throw exception
	{
		std::wfstream stream;
		open(file, stream, std::ios_base::in);
		std::wstring ret = read_content(stream);
		stream.close();
		return ret;
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

		if (flags & FILE_FLAG_CREATE_DIRECTORIES)
			create_directories(file.parent_path());

		std::wofstream stream;
		open(file, stream);

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

		std::wofstream out_stream;

		open(file, out_stream);
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

	std::wstring open_object(const std::wstring& object_id, std::wifstream& in_stream)
	{
		Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

		std::wstring object_type;
		in_stream >> object_type >> std::ws;

		return object_type;
	}

	bool object_exists(const std::wstring& object_id)
	{
		return fs::exists(get_object_path(object_id));
	}

	bool is_prefix(const fs::path& prefix, const fs::path& path)
	{
		auto res = std::mismatch(prefix.begin(), prefix.end(), path.begin());
		if (res.first == prefix.end())
			return true;
		else
			return false;
	}
	void make_sure_file_exists(const fs::path& file)
	{
		std::wofstream stream(file.wstring(), std::ios_base::app);
	}

	struct stat get_stat(const fs::path& file)
	{
		struct stat ret;
		stat(file.string().c_str(), &ret);
		return ret;
	}
}