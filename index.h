#pragma once

#include <ctime>
#include <string>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

enum UpdateIndexFlags
{
	UPDATE_INDEX_MODIFY = 1,
	UPDATE_INDEX_ADD	= 2,
	UPDATE_INDEX_REMOVE = 4,
	UPDATE_INDEX_FORCE_REMOVE = 12,
	UPDATE_INDEX_REMOVE_IF_DELETED = 16,
};

struct IndexRecord
{
	IndexRecord(std::string id, fs::path path) : id(id), path(path) {}
	IndexRecord() {}

	std::string	id;
	fs::path		path;
	time_t			mtime;
};

std::ostream& operator<<(std::ostream& ostream, const IndexRecord& record);
std::istream& operator>>(std::istream& istream, IndexRecord& record);

// used internally by update_index
// returns true, if the current record was overwritten by a new record
bool process_index_record(const IndexRecord& record, const fs::path& file, std::ostream& output, int flags);
void update_index(const std::vector<fs::path>& files, int flags);
void read_tree_into_index(std::ostream &out_stream, const std::string& tree_id);
void read_tree_into_index(const fs::path &index_file, const std::string& tree_id);
void read_tree_into_index(const fs::path& index_file, const std::string& tree1_id, const std::string& tree2_id);
