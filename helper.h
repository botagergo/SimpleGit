#pragma once

#include <string>
#include <windows.h>

#include <boost/filesystem/path.hpp>

#include "defs.h"

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

std::string get_default_git_editor();
std::string get_git_editor();

std::string get_commit_message();