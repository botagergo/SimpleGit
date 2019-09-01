#pragma once

#include <string>

void			write_branch(const std::wstring& name, const std::wstring& object_id, int flags = 0);
void			move_branch(const std::wstring& oldname, const std::wstring& newname, int flags = 0);
void			copy_branch(const std::wstring& oldname, const std::wstring& newname, int flags = 0);

// deletes the branch with the given name
// throws exception if the branch doesn't exist, or is checked out
void			delete_branch(const std::wstring& name, int flags = 0);
bool			is_branch(const std::wstring& name);
std::wstring	resolve_branch(const std::wstring& branch_name);
void			write_head(const std::wstring& name);
std::wstring	read_head();
std::wstring	resolve_head();
void			list_branches();