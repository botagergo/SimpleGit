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
	IndexRecord(std::wstring id, std::filesystem::path path) : id(id), path(path) {}
	IndexRecord() {}

	std::wstring id;
	std::filesystem::path path;
};

std::wostream& operator<<(std::wostream& ostream, const IndexRecord& record);
std::wistream& operator>>(std::wistream& istream, IndexRecord& record);

void update_index(const std::vector<std::filesystem::path>& files, int flags);