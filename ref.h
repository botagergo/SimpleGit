#pragma once

#include "defs.h"

// returns the object referenced by the parameter
// ref can be a tag or branch name, or the prefix of an object id
std::string resolve_ref(const std::string& ref);

// returns the object referenced by the tag
std::string resolve_tag(const std::string& tag_name);

// returns the commit id associated with the branch
std::string resolve_branch(const std::string& branch_name);

// resolves the parameter to a blob
std::string	resolve_to_blob(const std::string& ref);

// resolves the parameter to a tree
std::string	resolve_to_tree(const std::string& ref);

// resolves the parameter to a commit
std::string	resolve_to_commit(const std::string& ref);

// resolves the parameter to a branch
std::string resolve_to_branch(const std::string& ref);

// returns the current branch or the checked out commit, if the head is detached
std::string resolve_head();

// tries to resolve ref to a blob, returns true on success
bool		try_resolve_to_blob(const std::string& ref, std::string& blob_id);
bool		try_resolve_to_blob(const std::string& ref);

// tries to resolve ref to a tree, returns true on success
bool		try_resolve_to_tree(const std::string& ref, std::string& tree_id);
bool		try_resolve_to_tree(const std::string& ref);

// tries to resolve ref to a commit, returns true on success
bool		try_resolve_to_commit(const std::string& ref, std::string& commit_id);
bool		try_resolve_to_commit(const std::string& ref);

// tries to resolve ref to a branch, returns true on success
bool		try_resolve_to_branch(const std::string& ref, std::string& branch_name);
bool		try_resolve_to_branch(const std::string& ref);

// tries to get the corresponding object_id
// throws exception if doesn't exist or is ambiguous
std::string expand_object_id_prefix(const std::string& object_id_prefix);

// returns true if "prefix" is a prefix of "object_id"
bool		match_object_id(const std::string& prefix, const std::string& object_id);