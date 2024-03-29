#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/array.hpp>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "index_two_tree_iterator.h"
#include "repository.h"
#include "tree.h"

std::ostream& operator<<(std::ostream& ostream, const IndexRecord& record)
{
	return ostream << record.id << ' ' << record.mtime << ' ' << record.mode << ' ' << record.path.string();
}

std::istream& operator>>(std::istream& istream, IndexRecord& record)
{
	istream >> record.id >> record.mtime >> record.mode >> std::ws;
	std::string line;
	auto &ret = std::getline(istream, line);
	record.path = line;
	return ret;
}

IndexReader::IndexReader(const fs::path& index_file)
{
	if (!fs::exists(index_file))
		_end = true;
	else
	{
		Filesystem::open(index_file, _index_in);
		next();
	}
}

IndexRecord create_index_record(const Repository& repo, const fs::path& file)
{
	IndexRecord record;
	record.id = write_blob_from_file(repo, file);
	record.path = file;
	record.mtime = Filesystem::get_stat(file).st_mtime;
	record.mode = "100644";
	return record;
}

bool process_index_record(const Repository& repo, const IndexRecord& record, const fs::path& file, std::ostream& output, int flags)
{
	bool updated = false;

	if (record.path == file)
	{
		if (fs::exists(file))
		{
			if (flags & UPDATE_INDEX_MODIFY)
			{
				// we check the stat information to determine whether the file is newer than that in the index
				struct stat s = Filesystem::get_stat(file);
				if (s.st_mtime > record.mtime)
					output << create_index_record(repo, file) << '\n';
				else
					output << record << '\n';
			}
			else if (flags & UPDATE_INDEX_FORCE_REMOVE)
				{}
			else
				throw Exception(boost::format("file is already in the index: %1%") % file);
		}
		else
		{
			if (flags & UPDATE_INDEX_REMOVE_IF_DELETED)
				{}
			else
				throw Exception(boost::format("file doesn't exist: %1%") % file);

		}

		updated = true;
	}
	else if (file < record.path)
	{
		if (flags & UPDATE_INDEX_ADD)
			output << create_index_record(repo, file) << '\n';
		else
			throw FileNotInIndexException(file);
	}

	return updated;
}

void update_index(const Repository& repo, const std::vector<fs::path>& files, int flags)
{
#ifdef _DEBUG
	// check for invalid flag combination
	if ((flags & UPDATE_INDEX_REMOVE) && (flags & UPDATE_INDEX_MODIFY)
		|| (flags & UPDATE_INDEX_FORCE_REMOVE) && (flags & UPDATE_INDEX_ADD))
		assert(false);
#endif

	assert(files.size() > 0);

	Filesystem::check_file_exists(repo.indexFile);

	std::ifstream in_stream;
	Filesystem::open(repo.indexFile, in_stream);

	std::stringstream output;
	IndexRecord record;

	auto curr_file = files.begin();

	while (!in_stream.eof() && curr_file != files.end())
	{
		// we read the index file until we reach eof or find a matching file
		while (!(in_stream >> record).eof() && record.path < *curr_file)
			output << record << '\n';

		if (in_stream.eof())
			break;

		bool updated = false;

		// we process the files that are lexicographically ahead of the current record
		while (curr_file != files.end() && record.path >= *curr_file)
		{
			// process_index_record returns true, if the current file matches the current index record,
			// and the record was modified, in which case we don't output it again
			updated |= process_index_record(repo, record, *curr_file, (std::ostream&)output, flags);
			curr_file++;
		}

		if (!updated)
			output << record << '\n';
	}

	// finish reading the entire file
	while (!(in_stream >> record).eof())
		output << record << '\n';

	if (curr_file != files.end())
	{
		if (flags & UPDATE_INDEX_ADD)
		{
			while (curr_file != files.end())
			{
				output << create_index_record(repo, *curr_file) << '\n';
				curr_file++;
			}
		}
		else
			throw FileNotInIndexException(*curr_file);
	}

	in_stream.close();
	Filesystem::write_content(repo.indexFile, (std::istream&)output, Filesystem::FILE_FLAG_OVERWRITE);
}

void read_tree_into_index(const Repository& repo, std::ostream& out_stream, const std::string& tree_id)
{
	auto tree_reader = Object(repo, tree_id).get_tree_reader();

	TreeRecord record;
	while (*tree_reader >> record)
	{
		if (record.kind == "blob")
		{
			out_stream << IndexRecord(record.id, record.path, get_time_t_now(), record.mode) << '\n';
		}
		else
			read_tree_into_index(repo, out_stream, record.id);
	}
}

void read_tree_into_index(const Repository& repo, const fs::path& index_file, const std::string& tree_id)
{
	std::ofstream out_stream;
	Filesystem::open(index_file, out_stream);
	read_tree_into_index(repo, out_stream, tree_id);
}

void read_tree_into_index(const Repository& repo, const fs::path& index_file, const std::string& tree1_id, const std::string& tree2_id)
{
	bool clean = true; //todo
	fs::path tmp_index_file = index_file.string() + "_TMP";

	std::ifstream index_in_stream, tree1_in_stream, tree2_in_stream;
	std::ofstream index_out_stream;

	Filesystem::check_file_exists(index_file);
	Filesystem::open(index_file, index_in_stream);
	Filesystem::open(tmp_index_file, index_out_stream);

	auto tree1_reader = Object(repo, tree1_id).get_tree_reader();
	auto tree2_reader = Object(repo, tree2_id).get_tree_reader();

	IndexTwoTreeIterator<std::ifstream, TreeReader, TreeReader> iter(index_in_stream, *tree1_reader, *tree2_reader);
	for (;!iter.end(); iter.next())
	{
		if (!iter.hasIndex() && !iter.hasTree1() && iter.hasTree2())
			index_out_stream << IndexRecord(iter.tree2().id, iter.tree2().path, 0, iter.tree2().mode) << '\n';
		else if (!iter.hasIndex() && iter.hasTree1() && !iter.hasTree2())
			continue;
		else if (!iter.hasIndex() && iter.hasTree1() && iter.hasTree2())
			assert(false); // this should not be possible
		else if (!iter.hasTree1() && !iter.hasTree2())
			index_out_stream << iter.index() << '\n';
		else if (!iter.hasTree1() && iter.hasTree2() && iter.index().id == iter.tree2().id)
			index_out_stream << iter.index() << '\n';
		else if (!iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree2().id)
			throw MergeConflictException();
		else if (clean && iter.hasTree1() && !iter.hasTree2() && iter.index().id == iter.tree1().id)
			continue;
		else if (iter.hasTree1() && !iter.hasTree2())
			throw MergeConflictException();
		else if (iter.hasTree1() && iter.hasTree2() && iter.tree1().id == iter.tree2().id)
			index_out_stream << iter.index() << '\n';
		else if (iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree1().id && iter.tree1().id != iter.tree2().id)
			throw MergeConflictException();
		else if (iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree1().id && iter.index().id == iter.tree2().id && iter.tree1().id != iter.tree2().id)
			index_out_stream << iter.index() << '\n';
		else if (clean && iter.index().id == iter.tree1().id && iter.index().id != iter.tree1().id && iter.tree1().id != iter.tree2().id)
			index_out_stream << IndexRecord(iter.tree2().id, iter.tree2().path, 0, iter.tree2().mode) << '\n';
		else if (!clean && iter.index().id == iter.tree1().id && iter.index().id != iter.tree1().id && iter.tree1().id != iter.tree2().id)
			throw MergeConflictException();
		else
			assert(false);
	}

	index_in_stream.close();
	index_out_stream.close();
	fs::remove(index_file);
	fs::rename(tmp_index_file, index_file);
}
