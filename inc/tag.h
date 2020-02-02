#pragma once

#include "defs.h"

bool			is_tag(const std::string& name);
std::string	read_tag(const std::string& name);
void			write_tag(const std::string& tag_name, const std::string& commit, int flags);
void			delete_tag(const std::string& tag_name);
void			list_tags();