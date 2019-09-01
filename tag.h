#pragma once
bool			is_tag(const std::wstring& name);
std::wstring	read_tag(const std::wstring& name);
void			write_tag(const std::wstring& tag_name, const std::wstring& commit, int flags);
void			delete_tag(const std::wstring& tag_name);
std::wstring	resolve_tag(const std::wstring& name);
void			list_tags();