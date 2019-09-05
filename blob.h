#pragma once

#include <filesystem>
#include <string>

#include <boost/filesystem.hpp>

#include "object.h"

namespace fs = boost::filesystem;

std::string			write_blob(const std::string& content);
std::string			write_blob_from_file(const fs::path& filename);
void					read_blob(const std::string& id, const fs::path& file);
