#include <fstream>
#include <string>

#include "config.h"
#include "exception.h"
#include "filesystem.h"

static inline void ltrim(std::wstring& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

static inline void rtrim(std::wstring& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

static inline void trim(std::wstring& s) {
	ltrim(s);
	rtrim(s);
}

std::map<std::wstring, std::wstring> read_config(const fs::path& config_file)
{
	std::map<std::wstring, std::wstring> config;
	std::wfstream in_stream;

	Filesystem::open(config_file, in_stream, std::ios_base::in);
	std::wstring line;
	while (std::getline(in_stream, line))
	{
		size_t mid = line.find_first_of(L'=');

		std::wstring left = line.substr(0, mid);
		trim(left);

		std::wstring right = line.substr(mid + 1);
		trim(right);

		if (left.size() == 0 || right.size() == 0)
			throw InvalidConfigFileFormatException(config_file);

		config.insert(std::make_pair(left, right));
	}

	if (config.find(L"user.name") == config.end())
		throw UserNameNotFoundException();

	return config;
}
