#include <fstream>
#include <iostream>

#include "exception.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "object_file_writer.h"
#include "tree.h"

std::wstring write_tree()
{
	std::wifstream stream(Config::IndexFile);
	if (!stream)
		throw IndexFileNotFoundException();

	IndexRecord record;

	std::vector<std::filesystem::path> curr_components;
	std::filesystem::path prev_path;

	std::vector<std::vector<std::pair<std::wstring, std::filesystem::path>>> dir_history;
	std::vector<std::vector<std::pair<std::wstring, std::filesystem::path>>> file_history;

	file_history.resize(3);
	dir_history.resize(3);

	while (stream >> record)
	{

		auto curr_it = curr_components.begin();
		auto it = record.path.begin();

		if (curr_components.empty())
		{
			std::copy(record.path.begin(), record.path.end(), std::back_inserter(curr_components));
			prev_path = record.path;
			continue;
		}

		for (; curr_it != curr_components.end() && it != record.path.end() && *curr_it == *it; curr_it++, it++)
			;

		// a path to a file cannot be a subpath of a path to another file
		assert((curr_it == curr_components.end()) == (it == record.path.end()));

		int curr_depth = -1;

		for (auto it = prev_path.begin(); it != prev_path.end(); it++) // todo
			curr_depth++;

		auto last_component = curr_components.rbegin();

		if (dir_history.size() <= curr_depth)
		{
			file_history.resize(curr_depth + 1);
			dir_history.resize(curr_depth + 1);
		}

		if (prev_path != L"z")
		{
			std::wstring id = create_blob_from_file(prev_path);
			file_history[curr_depth].push_back(std::make_pair(id, prev_path));
		}


		while (*last_component != *curr_it)
		{
			last_component++;
			prev_path = prev_path.parent_path();

			std::wstring id = create_tree(file_history[curr_depth], dir_history[curr_depth]);
			file_history[curr_depth].clear(); dir_history[curr_depth].clear();

			curr_depth--;
			dir_history[curr_depth].push_back(std::make_pair(id, prev_path));
		}

		curr_components.resize(curr_it - curr_components.begin());
		std::copy(it, record.path.end(), std::back_inserter(curr_components));

		prev_path = record.path;
	}

	return create_tree(file_history[0], dir_history[0]);
}

std::wstring create_tree(const std::vector<std::pair<std::wstring, std::filesystem::path>>& files,
	const std::vector<std::pair<std::wstring, std::filesystem::path>>& dirs)
{
	ObjectFileWriter writer;

	for (auto file : files)
		writer << L"blob " << file.second.wstring() << L" " << file.first << L"\n";
	for (auto dir : dirs)
		writer << L"tree " << dir.second.wstring() << L" " << dir.first << L"\n";

	writer.save();
	return writer.id();
}
