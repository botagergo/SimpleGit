#pragma once

#include <string>

void merge_tree_into_index(const fs::path& index_file, const std::string& tree_id);

std::string merge_tree(const std::string& tree1_id, const std::string& tree2_id);