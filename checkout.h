#pragma once

#include <string>

// clears all the files in the repository except the git directory
void clear_working_directory();

// sets the working directory to the content of the tree
void set_working_directory(const std::wstring& tree_id);
void checkout(const std::wstring& commit_id);