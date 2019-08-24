#pragma once

#include <filesystem>
#include <string>

#include "object.h"

std::wstring			create_blob(const std::wstring &content);
std::wstring			create_blob_from_file(const std::filesystem::path &filename);