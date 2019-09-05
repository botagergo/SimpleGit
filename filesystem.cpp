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

	std::string read_content(const fs::path &file) // todo: throw exception
	{
		std::fstream stream;
		open(file, stream, std::ios_base::in);
		std::string ret = read_content(stream);
		stream.close();
		return ret;
	}

	void write_content(const fs::path &file, const std::string &content, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw FileExistsException(file);

		if (flags & FILE_FLAG_CREATE_DIRECTORIES)
			create_directories(file.parent_path());

		std::ofstream stream;
		open(file, stream);

		if (!stream)
			throw FileOpenException(file);

		stream << content;
		stream.close();

		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	void write_content(const fs::path& file, std::istream& in_stream, int flags)
	{
		if (!(flags & FILE_FLAG_OVERWRITE) && fs::exists(file))
			throw FileExistsException(file);

		std::ofstream out_stream;

		open(file, out_stream);
		write_content(out_stream, in_stream);
		out_stream.close();

		SetFileAttributes(file.c_str(), flags & (FILE_FLAG_READONLY | FILE_FLAG_HIDDEN));
	}

	void write_content(std::ostream &out_stream, std::istream &in_stream)
	{
		std::string line;
		while (std::getline(in_stream, line))
			out_stream << line << "\n";
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

	std::string open_object(const std::string& object_id, std::ifstream& in_stream)
	{
		Filesystem::open(Filesystem::get_object_path(object_id), in_stream, std::ios_base::in);

		std::string object_type;
		in_stream >> object_type >> std::ws;

		return object_type;
	}

	bool object_exists(const std::string& object_id)
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
		std::ofstream stream(file.string(), std::ios_base::app);
	}

	struct stat get_stat(const fs::path& file)
	{
		struct stat ret;
		stat(file.string().c_str(), &ret);
		return ret;
	}
}