#pragma once

#include <string>

void merge_tree_into_index(const fs::path& index_file, const std::wstring& tree_id);

std::wstring merge_tree(const std::wstring& tree1_id, const std::wstring& tree2_id);