#pragma once

#include "defs.h"

std::map<std::string, std::string> read_config(const fs::path& config_file);

void write_config(const fs::path& config_file, const std::map<std::string, std::string>& config);
void write_config(const fs::path& config_file, const std::string& name, const std::string& value);
