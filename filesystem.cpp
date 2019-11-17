#include <fstream>
#include <windows.h>

#include <boost/filesystem.hpp>

#include "exception.h"
#include "filesystem.h"

namespace fs = boost::filesystem;

namespace Filesystem
{
	void create_directory(const fs::path &dir, int flags)
	{
		if (dir.empty())
			return;

		boost::system::error_code ec;
		fs::create_directories(dir, ec);
		if(ec.failed())
			throw Exception(ec.message());
		
		if (flags & FILE_FLAG_HIDDEN)
			SetFileAttributes(dir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	std::string read_content(const fs::path &file)
	{
		std::ifstream stream;
		Filesystem::open(file, stream);
		return read_content(stream);
	}

	std::string read_content(std::istream& stream)
	{
		return std::string((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
	}

	std::vector<std::string> read_lines(std::istream& stream)
	{
		std::vector<std::string> ret;
		std::string line;

		while (std::getline(stream, line))
			ret.push_back(line);

		return ret;
	}

	std::vector<std::string> read_lines(const fs::path& file)
	{
		std::ifstream in_stream;
		Filesystem::open(file, in_stream);
		return read_lines(in_stream);
	}

	void write_content(const fs::path &file, const std::string &content, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw Exception(boost::format("file exists: ") % file);

		if (flags & FILE_FLAG_CREATE_DIRECTORY)
			create_directories(file.parent_path());

		std::ofstream stream;
		Filesystem::open(file, stream);

		if (!stream)
			throw Exception(boost::format("cannot open file: %1%") % file);

		stream << content;
		stream.close();

		if (flags & FILE_FLAG_READONLY)
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_READONLY);
		if (flags & FILE_FLAG_HIDDEN)
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void write_content(const fs::path& file, std::istream& in_stream, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw FileExistsException(file);

		std::ofstream out_stream;
		Filesystem::open(file, out_stream, flags);

		write_content(out_stream, in_stream);
		out_stream.close();

		if (flags & FILE_FLAG_READONLY)
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_READONLY);
		if (flags & FILE_FLAG_HIDDEN)
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void write_content(std::ostream &out_stream, std::istream &in_stream)
	{
		std::string line;
		while (std::getline(in_stream, line))
			out_stream << line << '\n';
	}

	fs::path get_object_path(const std::string& id)
	{
		fs::path dir = get_object_dir(id);
		fs::path filename = id.substr(Globals::IdPrefixLength);
		return dir / filename;
	}

	fs::path get_object_dir(const std::string& id)
	{
		fs::path prefix = id.substr(0, Globals::IdPrefixLength);
		return Globals::ObjectDir / prefix;
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
		std::ofstream stream(file.string(), std::ios_base::app);
	}

	struct stat get_stat(const fs::path& file)
	{
		struct stat ret;
		stat(file.string().c_str(), &ret);
		return ret;
	}
}