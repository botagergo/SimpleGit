#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "blob.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "index.h"
#include "index_two_tree_iterator.h"
#include "tree.h"

std::wostream& operator<<(std::wostream& ostream, const IndexRecord& record)
{
	return ostream << record.id << L' ' << record.mtime << L' ' << record.path.wstring();
}

std::wistream& operator>>(std::wistream& istream, IndexRecord& record)
{
	istream >> record.id >> record.mtime >> std::ws;
	std::wstring line;
	auto &ret = std::getline(istream, line);
	record.path = line;
	return ret;
}

IndexRecord create_index_record(const fs::path& file)
{
	IndexRecord record;
	record.id = write_blob_from_file(file);
	record.path = file;
	record.mtime = Filesystem::get_stat(file).st_mtime;
	return record;
}

bool process_index_record(const IndexRecord& record, const fs::path& file, std::wostream& output, int flags)
{
	bool updated = false;

	if (record.path == file)
	{
		if (flags & UPDATE_INDEX_MODIFY && fs::exists(file))
		{
			// we check the stat information to determine whether the file is newer than that in the index
			struct stat s = Filesystem::get_stat(file);
			if (s.st_mtime > record.mtime)
				output << create_index_record(file) << L'\n';
			else
				output << record << L'\n';
		}
		else if (flags & UPDATE_INDEX_REMOVE_IF_DELETED && !fs::exists(file))
			;
		else if ((flags & UPDATE_INDEX_FORCE_REMOVE) != UPDATE_INDEX_FORCE_REMOVE && fs::exists(record.path))
			throw IndexRemoveFileExists(file);
		else if (fs::exists(file))
			throw FileExistsException(file);
		else
			throw FileAlreadyInIndexException(file);
		updated = true;
	}
	else if (file < record.path)
	{
		if (flags & UPDATE_INDEX_ADD)
			output << create_index_record(file) << L'\n';
		else
			throw FileNotInIndexException(file);
	}

	return updated;
}

void update_index(const std::vector<fs::path>& files, int flags)
{
	// check for invalid flag combination
	if ((flags & UPDATE_INDEX_REMOVE) &&
		(flags & (UPDATE_INDEX_ADD | UPDATE_INDEX_MODIFY)))
		assert(false);

	assert(files.size() > 0);

	Filesystem::make_sure_file_exists(Globals::IndexFile);

	std::wifstream stream;
	Filesystem::open(Globals::IndexFile, stream);

	std::wstringstream output;
	IndexRecord record;

	auto curr_file = files.begin();

	while (!stream.eof() && curr_file != files.end())
	{
		// we read the index file until we reach eof or find a matching file
		while (!(stream >> record).eof() && record.path < *curr_file)
			output << record << L'\n';

		if (stream.eof())
			break;

		bool updated = false;

		while (curr_file != files.end() && record.path >= *curr_file)
		{
			updated |= process_index_record(record, *curr_file, (std::wostream&)output, flags);
			curr_file++;
		}

		if (!updated)
			output << record << L'\n';
	}

	// finish reading the entire file
	while (!(stream >> record).eof())
		output << record << L'\n';

	if (curr_file != files.end())
	{
		if (flags & UPDATE_INDEX_ADD)
		{
			while (curr_file != files.end())
			{
				output << create_index_record(*curr_file) << L'\n';
				curr_file++;
			}
		}
		else
			throw FileNotInIndexException(*curr_file);
	}

	stream.close();
	Filesystem::write_content(Globals::IndexFile, (std::wistream&)output, Filesystem::FILE_FLAG_OVERWRITE);
}

void read_tree_into_index(std::wostream& out_stream, const std::wstring& tree_id)
{
	std::wifstream in_stream;
	std::wstring object_kind = Filesystem::open_object(tree_id, in_stream);

	if (object_kind != L"tree")
		throw NotTreeException(tree_id);

	TreeRecord record;
	while (in_stream >> record)
	{
		if (record.kind == L"blob")
			out_stream << IndexRecord(record.id, record.path) << L'\n';
		else
			read_tree_into_index(out_stream, record.id);
	}
}

void read_tree_into_index(const fs::path& index_file, const std::wstring& tree_id)
{
	std::wofstream out_stream;
	Filesystem::open(index_file, out_stream);
	read_tree_into_index(out_stream, tree_id);
}

void read_tree_into_index(const fs::path& index_file, const std::wstring& tree1_id, const std::wstring& tree2_id)
{
	bool clean = true; //todo
	fs::path tmp_index_file = index_file.wstring() + L"_TMP";

	std::wifstream index_in_stream, tree1_in_stream, tree2_in_stream;
	std::wofstream index_out_stream;

	Filesystem::make_sure_file_exists(index_file);

	Filesystem::open(index_file, index_in_stream);
	if (Filesystem::open_object(tree1_id, tree1_in_stream) != L"tree")
		throw NotTreeException(tree1_id);
	if (Filesystem::open_object(tree2_id, tree2_in_stream) != L"tree")
		throw NotTreeException(tree2_id);

	Filesystem::open(tmp_index_file, index_out_stream);

	IndexTwoTreeIterator iter(index_in_stream, tree1_in_stream, tree2_in_stream);
	for (;!iter.end(); iter.next())
	{
		if (!iter.hasIndex() && !iter.hasTree1() && iter.hasTree2())
			index_out_stream << IndexRecord(iter.tree2().id, iter.tree2().path) << L'\n';
		else if (!iter.hasIndex() && iter.hasTree1() && !iter.hasTree2())
			continue;
		else if (!iter.hasIndex() && iter.hasTree1() && iter.hasTree2())
			assert(false); // this should not be possible
		else if (!iter.hasTree1() && !iter.hasTree2())
			index_out_stream << iter.index() << L'\n';
		else if (!iter.hasTree1() && iter.hasTree2() && iter.index().id == iter.tree2().id)
			index_out_stream << iter.index() << L'\n';
		else if (!iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree2().id)
			throw MergeConflictException();
		else if (clean && iter.hasTree1() && !iter.hasTree2() && iter.index().id == iter.tree1().id)
			continue;
		else if (iter.hasTree1() && !iter.hasTree2())
			throw MergeConflictException();
		else if (iter.hasTree1() && iter.hasTree2() && iter.tree1().id == iter.tree2().id)
			index_out_stream << iter.index() << L'\n';
		else if (iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree1().id && iter.tree1().id != iter.tree2().id)
			throw MergeConflictException();
		else if (iter.hasTree1() && iter.hasTree2() && iter.index().id != iter.tree1().id && iter.index().id == iter.tree2().id && iter.tree1().id != iter.tree2().id)
			index_out_stream << iter.index() << L'\n';
		else if (clean && iter.index().id == iter.tree1().id && iter.index().id != iter.tree1().id && iter.tree1().id != iter.tree2().id)
			index_out_stream << IndexRecord(iter.tree2().id, iter.tree2().path) << L'\n';
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