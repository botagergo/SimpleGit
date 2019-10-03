#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "object.h"
#include "tree.h"


std::ostream& operator<<(std::ostream& ostream, const TreeRecord& record)
{
	return ostream << record.kind << " " << record.id << " " << record.path.string();
}

std::istream& operator>>(std::istream& istream, TreeRecord& record)
{
	istream >> record.mode >> std::ws;
	istream >> record.kind >> record.id >> std::ws;
	std::string line;
	auto& ret = std::getline(istream, line);
	record.path = line;
	return ret;
}

std::string write_tree(const fs::path& prefix, bool ignore_missing)
{
	std::string ret_id;

	auto create_index_objects = [&](IndexRecord record, size_t diff_pos, std::vector<std::vector<TreeRecord>>& history)
	{
		auto it = record.path.begin();
		size_t size = 0;
		for (; size <= diff_pos; it++, size++)
			;

		for (; it != record.path.end(); it++)
			size++;

		size_t curr_level = size - 1;
		if (history.size() <= curr_level)
			history.resize(curr_level + 1);

		if (!ignore_missing && !Object(record.id).exists())
			throw ObjectNotFoundException(record.id);

		history[curr_level].push_back(TreeRecord("blob", "100644", record.id, record.path));

		while (curr_level > diff_pos)
		{
			record.path = record.path.parent_path();

			std::string id = write_tree(history[curr_level]);
			history[curr_level].clear();

			curr_level--;
			if (record.path == prefix)
				ret_id = id;
			history[curr_level].push_back(TreeRecord("tree", "040000", id, record.path));
		}
	};

	std::ifstream index_in_stream(Globals::IndexFile.string());
	if (!index_in_stream)
		throw IndexFileNotFoundException();

	IndexRecord record;
	IndexRecord prev_record;
	std::vector<std::vector<TreeRecord>> history(3);

	index_in_stream >> record;

	if (!index_in_stream)
		throw IndexFileEmptyException();

	prev_record = record;

	while (index_in_stream >> record)
	{
		auto prev_it = prev_record.path.begin();
		auto curr_it = record.path.begin();

		size_t diff_pos = 0;
		for (; prev_it != prev_record.path.end() && curr_it != record.path.end() && *prev_it == *curr_it; prev_it++, curr_it++)
			diff_pos++;

		assert(prev_it != prev_record.path.end() && curr_it != record.path.end());

		create_index_objects(prev_record, diff_pos, history);
		prev_record = record;
	}

	create_index_objects(prev_record, 0, history);
	std::string root_id = write_tree(history[0]);

	if (!ret_id.empty())
		return ret_id;
	else
		return root_id;
}

std::string write_tree(const std::vector<TreeRecord>& records)
{
	ObjectWriter writer("tree");

	for (const TreeRecord& record : records)
	{
		writer << record.mode << ' ' << record.path.string() << '\0';

		for (int i = 0; i < 20; i++)
		{
			writer << (char)((hex_char_to_byte(record.id[2 * i]) << 4) | hex_char_to_byte(record.id[2 * i + 1]));
		}
	}

	return writer.save();
}

void read_tree(const std::string& tree_id, const fs::path& root_dir)
{
	auto tree_reader = Object(tree_id).get_tree_reader();

	TreeRecord record;
	while (*tree_reader >> record)
	{
		if (record.kind == "blob")
			read_blob(record.id, root_dir / record.path);
		else if (record.kind == "tree")
			read_tree(record.id, root_dir);
		else
			throw Exception(boost::format("invalid tree record kind: ") % record.kind);
	}
}

TreeReader& TreeReader::operator>>(TreeRecord& record)
{
	if (_curr_pos >= _end)
	{
		_valid = false;
		return *this;
	}

	char mode[10];
	sscanf_s(_curr_pos, "%s", mode, 10);
	record.mode = mode;
	_curr_pos += strlen(mode) + 1;
	record.path = _curr_pos;
	_curr_pos += record.path.size() + 1;
	record.id.resize(40);
	for (int i = 0; i < 20; i++)
	{
		unsigned char ch = _curr_pos[i];
		record.id[2 * i] = int_to_hex_char(ch >> 4);
		record.id[2 * i + 1] = int_to_hex_char(ch & 15);
	}
	_curr_pos += 20;
	record.kind = record.mode[0] == '1' ? "blob" : "tree";

	return *this;
}
