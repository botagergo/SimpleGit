#pragma once

#include "defs.h"

std::string resolve_ref(const std::string& ref);
std::string resolve_tag(const std::string& tag_name);
std::string resolve_branch(const std::string& branch_name);
std::string	resolve_tree(const std::string& name);
std::string resolve_head();

// tries to get the corresponding object_id
// throws exception if doesn't exist or is ambiguous
std::string expand_object_id_prefix(const std::string& object_id_prefix);

// returns true if "prefix" is a prefix of "object_id"
bool		match_object_id(const std::string& prefix, const std::string& object_id);