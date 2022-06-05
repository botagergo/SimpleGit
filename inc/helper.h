#pragma once

#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include "defs.h"

namespace po = boost::program_options;

std::string get_hash(const char* text, size_t size);
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

char int_to_hex_char(unsigned a);
char hex_char_to_byte(char ch);

time_t get_time_t_now();

fs::path    get_home_directory();
std::string get_username();
std::string get_git_editor();
std::string get_default_git_editor();

void check_error_code(const boost::system::error_code& ec, const std::string& arg="");
void check_error_code(const std::error_code& ec, const std::string& arg="");
