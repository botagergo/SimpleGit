#pragma once

class Repository;

void			write_branch(const Repository& repo, const std::string& name, const std::string& object_id, int flags = 0);
void			move_branch(const Repository& repo, const std::string& oldname, const std::string& newname, int flags = 0);
void			copy_branch(const Repository& repo, const std::string& oldname, const std::string& newname, int flags = 0);

// deletes the branch with the given name
// throws exception if the branch doesn't exist, or is checked out
void			delete_branch(const Repository& repo, const std::string& name, int flags = 0);
bool			is_branch(const Repository& repo, const std::string& name);
void			write_head(const Repository& repo, const std::string& name);
std::string		read_head(const Repository& repo);
void			list_branches(const Repository& repo);
