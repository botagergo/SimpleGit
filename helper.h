#pragma once

#include <filesystem>
#include <string>

#include "globals.h"
#include "istream_line_iterator.h"

namespace fs = boost::filesystem;

std::wstring to_wide_string(const std::string &str);
std::string to_string(const std::wstring &str);

std::string get_hash(const std::string &text);
std::string get_file_hash(const fs::path &file);

template<class ForwardIt, class T>
ForwardIt binary_search(ForwardIt first, ForwardIt last, const T& value)
{
	first = std::lower_bound(first, last, value);
	if (!(first == last) && !(value < *first))
		return first;
	else
		return last;
}