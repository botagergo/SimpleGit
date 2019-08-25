#pragma once

#include <string>

enum UpdateIndexFlags
{
	UPDATE_INDEX_MODIFY = 1,
	UPDATE_INDEX_ADD	= 2,
	UPDATE_INDEX_REMOVE = 4,
	UPDATE_INDEX_FORCE_REMOVE = 12,
};

struct IndexRecord
{
	IndexRecord(std::wstring id, fs::path path) : id(id), path(path) {}
	IndexRecord() {}

	std::wstring id;
	fs::path path;
};

std::wostream& operator<<(std::wostream& ostream, const IndexRecord& record);
std::wistream& operator>>(std::wistream& istream, IndexRecord& record);

void update_index(const std::vector<fs::path>& files, int flags);
void write_index(std::wostream &out_stream, const std::wstring& tree_id);
void write_index(const fs::path &index_file, const std::wstring& tree_id);