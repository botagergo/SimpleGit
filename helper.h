#pragma once

#include <filesystem>
#include <string>

#include "globals.h"
#include "istream_line_iterator.h"

namespace fs = std::filesystem;

std::wstring to_wide_string(const std::string &str);
std::string to_string(const std::wstring &str);

std::wstring get_hash(const std::wstring &text);
std::wstring get_file_hash(const fs::path &file);