#include <fstream>
#include <iostream>

#include "blob.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "object.h"
#include "object_file_writer.h"
#include "tree.h"

std::ostream& operator<<(std::ostream& ostream, const TreeRecord& record)
{
	return ostream << record.kind << ' ' << record.id << ' ' << record.path.string();
}

std::istream& operator>>(std::istream& istream, TreeRecord& record)
{
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

		if (!ignore_missing && !Filesystem::object_exists(record.id))
			throw ObjectNotFoundException(record.id);

		history[curr_level].push_back(TreeRecord("blob", record.id, record.path));

		while (curr_level > diff_pos)
		{
			record.path = record.path.parent_path();

			std::string id = write_tree(history[curr_level]);
			history[curr_level].clear();

			curr_level--;
			if (record.path == prefix)
				ret_id = id;
			history[curr_level].push_back(TreeRecord("tree", id, record.path));
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

void read_tree(const std::string& tree_id, const fs::path& root_dir)
{
	std::ifstream in_stream;
	Filesystem::open(Filesystem::get_object_path(tree_id), in_stream);

	std::string object_type;
	in_stream >> object_type;

	if (object_type != "tree")
		throw NotTreeException(tree_id);

	TreeRecord record;
	while (in_stream >> record)
	{
		if (record.kind == "blob")
			read_blob(record.id, root_dir / record.path);
		else if (record.kind == "tree")
			read_tree(record.id, root_dir);
		else
			throw TreeFileCorrupted(tree_id);
	}
}

std::string write_tree(const std::vector<TreeRecord>& records)
{
	ObjectFileWriter writer;

	writer << "tree" << '\n';

	for (const TreeRecord& record : records)
		writer << record << "\n";

	return writer.save();
}

std::string resolve_tree(const std::string& name)
{
	if (name.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(name);

		std::ifstream in_stream;
		std::string object_type = open_object(in_stream, object_id);

		if (object_type == "commit")
		{
			Commit commit = read_commit(in_stream);
			return commit.tree_id;
		}
		else if (object_type == "tree")
			return object_id;
		else
			throw NotTreeException(name);
	}

	throw ResolveObjectException(name);
}

void open_tree(std::ifstream& in_stream, const std::string& tree_id)
{
	std::string object_type = open_object(in_stream, tree_id);
	if (object_type != "tree")
		throw NotTreeException(tree_id);
}
