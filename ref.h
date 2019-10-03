#pragma once

#include "defs.h"

std::string resolve_ref(const std::string& ref);
std::string resolve_tag(const std::string& tag_name);
std::string resolve_branch(const std::string& branch_name);
std::string	resolve_blob(const std::string& ref);
std::string	resolve_to_tree(const std::string& name);
std::string	resolve_to_commit(const std::string& ref);

std::string resolve_to_object(const std::string& ref);

std::string resolve_head();

bool		try_resolve_to_blob(const std::string& ref, std::string& blob_id);
bool		try_resolve_to_blob(const std::string& ref);

bool		try_resolve_to_tree(const std::string& ref, std::string& tree_id);
bool		try_resolve_to_tree(const std::string& ref);

bool		try_resolve_to_commit(const std::string& ref, std::string& commit_id);
bool		try_resolve_to_commit(const std::string& ref);

// tries to get the corresponding object_id
// throws exception if doesn't exist or is ambiguous
std::string expand_object_id_prefix(const std::string& object_id_prefix);

// returns true if "prefix" is a prefix of "object_id"
bool		match_object_id(const std::string& prefix, const std::string& object_id);