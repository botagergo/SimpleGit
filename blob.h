#pragma once

#include <filesystem>
#include <string>

#include <boost/filesystem.hpp>

#include "object.h"

namespace fs = boost::filesystem;

std::wstring			write_blob(const std::wstring& content);
std::wstring			write_blob_from_file(const fs::path& filename);
void					read_blob(const std::wstring& id, const fs::path& file);
