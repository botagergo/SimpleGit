#include <fstream>
#include <map>
#include <string>
#include "locale.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>

#include "config.h"
#include "exception.h"
#include "filesystem.h"

#include "platform.h"

namespace Config
{
	void read(std::map<std::string, std::string>& config, const fs::path& config_file)
	{
		std::fstream in_stream;

		Filesystem::open(config_file, in_stream);
		std::string line;
		while (std::getline(in_stream, line))
		{
			size_t mid = line.find_first_of('=');

			std::string left = line.substr(0, mid);
			boost::algorithm::trim(left);

			std::string right = line.substr(mid + 1);
			boost::algorithm::trim(right);

			if (left.size() == 0)
				throw InvalidConfigFileFormatException(config_file);

			config[left] = right;
		}
	}

	std::map<std::string, std::string> read(const fs::path& config_file)
	{
		std::map<std::string, std::string>  config;
		read(config, config_file);
		return config;
	}

	void write(const fs::path& config_file, const std::map<std::string, std::string>& config)
	{
		Filesystem::check_file_exists(config_file);

		std::ofstream out_stream;
		Filesystem::open(config_file, out_stream);

		for (auto p : config)
			out_stream << p.first << '=' << p.second << '\n';
	}

	void write(const fs::path& config_file, const std::string& name, const std::string& value)
	{
		Filesystem::check_file_exists(config_file);

		auto config = read(config_file);
		config[name] = value;
		write(config_file, config);
	}

	void init()
	{
		Globals::Config.clear();

		if (fs::exists(Globals::GlobalConfigFile))
			Config::read(Globals::Config, Globals::GlobalConfigFile);
		if (fs::exists(Globals::ConfigFile))
			Config::read(Globals::Config, Globals::ConfigFile);

		if (Globals::Config.find("user.name") == Globals::Config.end())
			Globals::Config["user.name"] = get_username();

		if (Globals::Config.find("debug") != Globals::Config.end())
			Globals::Debug = true;
		else
			Globals::Debug = false;
	}

	bool find(const std::string& name)
	{
		return Globals::Config.find(name) != Globals::Config.end();
	}

	std::string get(const std::string& name)
	{
		auto value = Globals::Config.find(name);
		if (value != Globals::Config.end())
			return value->second;
		else
			throw ConfigNotFoundException(name);
		
	}
}