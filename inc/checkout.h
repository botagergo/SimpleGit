#pragma once

class Repository;

// clears all the files in the repository except the git directory
void clear_working_directory(const Repository& repo);

// sets the working directory to the content of the tree
void set_working_directory(const Repository& repo, const std::string& tree_id);
void checkout(const Repository& repo, const std::string& commit_id, bool keep_working_directory = false, bool keep_index = false);
