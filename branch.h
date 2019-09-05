#pragma once

#include <string>

void			write_branch(const std::string& name, const std::string& object_id, int flags = 0);
void			move_branch(const std::string& oldname, const std::string& newname, int flags = 0);
void			copy_branch(const std::string& oldname, const std::string& newname, int flags = 0);

// deletes the branch with the given name
// throws exception if the branch doesn't exist, or is checked out
void			delete_branch(const std::string& name, int flags = 0);
bool			is_branch(const std::string& name);
std::string	resolve_branch(const std::string& branch_name);
void			write_head(const std::string& name);
std::string	read_head();
std::string	resolve_head();
void			list_branches();