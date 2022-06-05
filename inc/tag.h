#pragma once

#include "defs.h"

bool			is_tag(const Repository& repo, const std::string& name);
std::string		read_tag(const Repository& repo, const std::string& name);
void			write_tag(const Repository& repo, const std::string& tag_name, const std::string& commit, int flags);
void			delete_tag(const Repository& repo, const std::string& tag_name);
void			list_tags(const Repository& repo);
