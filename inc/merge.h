#pragma once

#include <string>

#include "defs.h"

void		merge_tree_into_index(const Repository& repo, const fs::path& index_file, const std::string& tree_id);
std::string merge_tree(const Repository& repo, const std::string& tree1_id, const std::string& tree2_id);
