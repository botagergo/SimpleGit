#pragma once

#include <map>
#include <string>
#include <vector>
#include <cassert>

#include "blob.h"
#include "defs.h"
#include "object.h"

struct TreeRecord
{
	TreeRecord(const std::string& kind, const std::string& mode, const std::string& id, const fs::path &path) 
		: kind(kind), mode(mode), id(id), path(path) {}
	TreeRecord() {}

	std::string			mode;
	std::string			kind;
	std::string			id;
	fs::path			path;
};

// pretty-prints a tree record
std::ostream& operator<<(std::ostream& ostream, const TreeRecord& record);

// parses a tree record
//
// record format:
// <tree_record> = <mode><SP><path><NULL><sha1>
// <mode> := [0-7]+
// <sha1> := 20 digit binary encoded hash number
std::istream& operator>>(std::istream& istream, TreeRecord& record);

class TreeReader : public ObjectReader
{
	friend class Object;

public:
	TreeReader& operator>>(TreeRecord& record);

	virtual std::ostream& pretty_print(std::ostream& out_stream) override
	{
		TreeRecord record;
		while (*this >> record)
			out_stream << record << '\n';
		return out_stream;
	}

protected:
	TreeReader(const ObjectData& data, const Object& tree) : ObjectReader(data, tree) {}
};

// creates a new tree from the current index
// ignore_missing	-	adds blob to the tree even if it doesn't exist
std::string write_tree(const fs::path& prefix = "", bool ignore_missing = false);

// creates a new tree from the given tree records
std::string	write_tree(const std::vector<TreeRecord>& records);

// reads a tree into the working directory
void		read_tree(const std::string& tree_id, const fs::path& root_dir = "");