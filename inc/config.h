#pragma once

#include "defs.h"

namespace Config
{
    void read(std::map<std::string, std::string>& config, const fs::path& config_file);
    std::map<std::string, std::string> read(const fs::path& config_file);

    void write(const fs::path& config_file, const std::map<std::string, std::string>& config);
    void write(const fs::path& config_file, const std::string& name, const std::string& value);

    void init();

    bool find(const std::string& name);
    std::string get(const std::string& name);
}