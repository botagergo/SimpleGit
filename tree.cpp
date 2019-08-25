#include <fstream>
#include <iostream>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "object_file_writer.h"
#include "tree.h"

std::wostream& operator<<(std::wostream& ostream, const TreeRecord& record)
{
	return ostream << record.kind << L' ' << record.id << L' ' << record.path.wstring();
}

std::wistream& operator>>(std::wistream& istream, TreeRecord& record)
{
	istream >> record.kind >> record.id >> std::ws;
	std::wstring line;
	auto& ret = std::getline(istream, line);
	record.path = line;
	return ret;
}

std::wstring write_tree()
{
	auto create_index_objects = [](fs::path path, size_t diff_pos, std::vector<std::vector<TreeRecord>>& history)
	{
		auto it = path.begin();
		size_t size = 0;
		for (; size <= diff_pos; it++, size++)
			;

		for (; it != path.end(); it++)
			size++;

		size_t curr_level = size - 1;
		if (history.size() <= curr_level)
			history.resize(curr_level + 1);

		std::wstring id = write_blob_from_file(path);
		history[curr_level].push_back(TreeRecord(L"blob", id, path));

		while (curr_level > diff_pos)
		{
			path = path.parent_path();

			id = write_tree(history[curr_level]);
			history[curr_level].clear();

			curr_level--;
			history[curr_level].push_back(TreeRecord(L"tree", id, path));
		}
	};

	std::wifstream stream(Globals::IndexFile);
	if (!stream)
		throw IndexFileNotFoundException();

	IndexRecord record;
	fs::path prev_path;
	std::vector<std::vector<TreeRecord>> history(3);

	stream >> record;
	if (!stream)
		throw IndexFileEmptyException();

	prev_path = record.path;

	while (stream >> record)
	{
		auto prev_it = prev_path.begin();
		auto curr_it = record.path.begin();

		size_t diff_pos = 0;
		for (; prev_it != prev_path.end() && curr_it != record.path.end() && *prev_it == *curr_it; prev_it++, curr_it++)
			diff_pos++;

		assert(prev_it != prev_path.end() && curr_it != record.path.end());

		create_index_objects(prev_path, diff_pos, history);
		prev_path = record.path;
	}

	create_index_objects(prev_path, 0, history);
	return write_tree(history[0]);
}

void read_tree(const std::wstring& tree_id, const fs::path& root_dir)
{
	std::wfstream in_stream;
	Filesystem::open(Filesystem::get_object_path(tree_id), in_stream, std::ios::in);

	std::wstring object_type;
	in_stream >> object_type;

	if (object_type != L"tree")
		throw NotTreeException(tree_id);

	TreeRecord record;
	while (in_stream >> record)
	{
		if (record.kind == L"blob")
			read_blob(record.id, root_dir / record.path);
		else if (record.kind == L"tree")
			read_tree(record.id, root_dir);
		else
			throw TreeFileCorrupted(tree_id);
	}
}

std::wstring write_tree(const std::vector<TreeRecord>& records)
{
	ObjectFileWriter writer;

	writer << L"tree" << L'\n';

	for (const TreeRecord& record : records)
		writer << record << L"\n";

	return writer.save();
}
