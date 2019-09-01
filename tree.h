#pragma once

#include <map>
#include <string>
#include <vector>

#include "blob.h"
#include <cassert>

struct TreeRecord
{
	TreeRecord(std::wstring kind, const std::wstring& id, const fs::path &path) : kind(kind), id(id), path(path) {}
	TreeRecord() {}

	std::wstring			kind;
	std::wstring			id;
	fs::path				path;
};

std::wostream& operator<<(std::wostream& ostream, const TreeRecord& record);
std::wistream& operator>>(std::wistream& istream, TreeRecord& record);

// creates a new tree from the current index
std::wstring write_tree(const fs::path& prefix = L"", bool ignore_missing = false);

// creates a new tree from the given tree records
std::wstring	write_tree(const std::vector<TreeRecord>& records);

// reads a tree into the specified directory
void			read_tree(const std::wstring& tree_id, const fs::path& root_dir = L"");

// returns the id of the tree associated with the given tree-ish
std::wstring	resolve_tree(const std::wstring& name);

void			open_tree(std::wifstream& in_stream, const std::wstring& tree_id);

void			pretty_print_tree(std::wostream& out_stream, std::wistream& in_stream);