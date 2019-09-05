#include <fstream>
#include <string>
#include "locale.h"

#include "config.h"
#include "exception.h"
#include "filesystem.h"

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !isspace(ch);
		}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !isspace(ch);
		}).base(), s.end());
}

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

std::map<std::string, std::string> read_config(const fs::path& config_file)
{
	std::map<std::string, std::string> config;
	std::fstream in_stream;

	Filesystem::open(config_file, in_stream, std::ios_base::in);
	std::string line;
	while (std::getline(in_stream, line))
	{
		size_t mid = line.find_first_of('=');

		std::string left = line.substr(0, mid);
		trim(left);

		std::string right = line.substr(mid + 1);
		trim(right);

		if (left.size() == 0 || right.size() == 0)
			throw InvalidConfigFileFormatException(config_file);

		config.insert(std::make_pair(left, right));
	}

	if (config.find("user.name") == config.end())
		throw UserNameNotFoundException();

	return config;
}
