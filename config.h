#pragma once

#include <filesystem>
#include <map>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::map<std::wstring, std::wstring> read_config(const fs::path& config_file);