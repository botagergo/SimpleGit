#include <fstream>
#include <filesystem>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "exception.h"
#include "filesystem.h"

namespace fs = boost::filesystem;

namespace Filesystem
{
	void set_readonly(const fs::path& path)
	{
		using boost::filesystem::perms;
		boost::filesystem::permissions(path, perms::remove_perms | perms::owner_write | perms::group_write | perms::others_write);
	}


	void create_directory(const fs::path &dir, int flags)
	{
		if (dir.empty())
			return;

		boost::system::error_code ec;
		fs::create_directories(dir, ec);
		if(ec.value() != boost::system::errc::success)
			throw Exception(ec.message());
		
		if (flags & FILE_FLAG_HIDDEN)
			set_hidden(dir.c_str());
	}

	void copy_directory(const fs::path& src, const fs::path& dest, int flags)
	{
		std::filesystem::copy_options options = std::filesystem::copy_options::none;
		if (flags & FILE_FLAG_OVERWRITE)
			options |= std::filesystem::copy_options::overwrite_existing;
		if (flags & FILE_FLAG_RECURSIVE)
			options |= std::filesystem::copy_options::recursive;
		
		std::filesystem::copy(src.string(), dest.string(), options);
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
			throw Exception(boost::format("file exists: %1%") % file);

		if (flags & FILE_FLAG_CREATE_DIRECTORY)
			create_directories(file.parent_path());

		std::ofstream stream;
		Filesystem::open(file, stream);

		if (!stream)
			throw Exception(boost::format("cannot open file: %1%") % file);

		stream << content;
		stream.close();

		if (flags & FILE_FLAG_READONLY)
			set_readonly(file.c_str());
		if (flags & FILE_FLAG_HIDDEN)
			set_hidden(file.c_str());
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
			set_readonly(file.c_str());
		if (flags & FILE_FLAG_HIDDEN)
			set_hidden(file.c_str());
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
