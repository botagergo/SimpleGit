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

std::map<std::string, std::string> read_config(const fs::path& config_file)
{
	if (!fs::exists(config_file))
		return {};

	std::map<std::string, std::string> config;
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

		config.insert(std::make_pair(left, right));
	}

	return config;
}

void write_config(const fs::path& config_file, const std::map<std::string, std::string>& config)
{
	Filesystem::make_sure_file_exists(config_file);

	std::ofstream out_stream;
	Filesystem::open(config_file, out_stream);

	for (auto p : config)
		out_stream << p.first << '=' << p.second << '\n';
}

void write_config(const fs::path& config_file, const std::string& name, const std::string& value)
{
	Filesystem::make_sure_file_exists(config_file);

	auto config = read_config(config_file);
	config[name] = value;
	write_config(config_file, config);
}

void init_config()
{
	Globals::Config = read_config(Globals::ConfigFile);
	if (Globals::Config.find("user.name") == Globals::Config.end())
		Globals::Config["user.name"] = get_username();

	if (Globals::Config.find("debug") != Globals::Config.end())
		Globals::Debug = true;
	else
		Globals::Debug = false;
}