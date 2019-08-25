#pragma once

#include <filesystem>
#include <map>

namespace fs = std::filesystem;

std::map<std::wstring, std::wstring> read_config(const fs::path& config_file);