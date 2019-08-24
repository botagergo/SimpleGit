#pragma once

#include <map>
#include <string>
#include <vector>

#include "blob.h"
#include "object.h"
#include <cassert>

// creates a tree from the current index
// todo: optimize
std::wstring write_tree();

std::wstring create_tree(const std::vector<std::pair<std::wstring, std::filesystem::path>>& files,
	const std::vector<std::pair<std::wstring, std::filesystem::path>>& dirs);