#pragma once

#include <map>
#include <string>
#include <vector>

#include "blob.h"
#include <cassert>

struct TreeRecord
{
	TreeRecord(std::string kind, const std::string& id, const fs::path &path) : kind(kind), id(id), path(path) {}
	TreeRecord() {}

	std::string			kind;
	std::string			id;
	fs::path				path;
};

std::ostream& operator<<(std::ostream& ostream, const TreeRecord& record);
std::istream& operator>>(std::istream& istream, TreeRecord& record);

// creates a new tree from the current index
std::string write_tree(const fs::path& prefix = "", bool ignore_missing = false);

// creates a new tree from the given tree records
std::string	write_tree(const std::vector<TreeRecord>& records);

// reads a tree into the specified directory
void			read_tree(const std::string& tree_id, const fs::path& root_dir = "");

// returns the id of the tree associated with the given tree-ish
std::string	resolve_tree(const std::string& name);

void			open_tree(std::ifstream& in_stream, const std::string& tree_id);

template <typename OutStream>
void pretty_print_tree(OutStream& out_stream, std::istream& in_stream)
{
	TreeRecord record;
	while (in_stream >> record)
		out_stream << record.kind << '\t' << record.id << '\t' << record.path << '\n';
}