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

std::wostream& operator<<(std::wostream& ostream, const IndexRecord& record)
{
	return ostream << record.id << L' ' << record.path.wstring();
}

std::wistream& operator>>(std::wistream& istream, IndexRecord& record)
{
	istream >> record.id >> std::ws;
	std::wstring line;
	auto &ret = std::getline(istream, line);
	record.path = line;
	return ret;
}

//void update_index(const std::vector<std::filesystem::path> &files, int flags)
//{
//	// check for invalid flag combination
//	if ((flags & UPDATE_INDEX_REMOVE) &&
//		(flags & (UPDATE_INDEX_ADD | UPDATE_INDEX_MODIFY)))
//		assert(false);
//
//	// make sure the index file exists
//	std::wfstream stream(Config::IndexFile, std::fstream::app);
//	stream.close();
//
//	stream.open(Config::IndexFile, std::fstream::in);
//	if (!stream)
//		throw IndexFileNotFoundException();
//
//	std::wstringstream output;
//	IndexRecord record;
//
//	auto curr_file = files.begin();
//
//	// we read the index file until we reach eof or find a matching file
//	while (!(stream >> record).eof() && record.path != *curr_file)
//		output << record << std::endl;
//
//	if (record.path == *curr_file)
//	{
//		// UPDATE_INDEX_MODIFY: output the new record
//		// UPDATE_INDEX_ADD: throw exception
//		// UPDATE_INDEX_REMOVE: skip the record
//		if (flags & UPDATE_INDEX_MODIFY)
//			output << IndexRecord(create_blob_from_file(filename), filename) << std::endl;
//		else if (flags & UPDATE_INDEX_ADD)
//			throw FileAlreadyInIndexException(filename);
//		else if ((flags & UPDATE_INDEX_FORCE_REMOVE) != UPDATE_INDEX_FORCE_REMOVE && std::filesystem::exists(record.path))
//			throw IndexRemoveFileExists(filename);
//
//		// finish reading the entire file
//		while (!(stream >> record).eof())
//			output << record << std::endl;
//	}
//	else
//	{
//		if (flags & UPDATE_INDEX_ADD)
//			output << IndexRecord(create_blob_from_file(filename), filename);
//		else
//			throw FileNotInIndexException(filename);
//	}
//
//	std::wstring line;
//	stream.close();
//	stream.open(Config::IndexFile, std::fstream::out);
//	while (std::getline(output, line))
//		stream << line << std::endl;
//}

void update_index(const std::vector<std::filesystem::path>& files, int flags)
{
	// check for invalid flag combination
	if ((flags & UPDATE_INDEX_REMOVE) &&
		(flags & (UPDATE_INDEX_ADD | UPDATE_INDEX_MODIFY)))
		assert(false);

	assert(files.size() > 0);

	// make sure the index file exists
	std::wfstream stream(Config::IndexFile, std::fstream::app);
	stream.close();

	stream.open(Config::IndexFile, std::fstream::in);
	if (!stream)
		throw IndexFileNotFoundException();

	std::wstringstream output;
	IndexRecord record;

	auto curr_file = files.begin();

	while (!stream.eof() && curr_file != files.end())
	{
		// we read the index file until we reach eof or find a matching file
		while (!(stream >> record).eof() && record.path < *curr_file)
			output << record << L"\n";

		if (stream.eof())
			break;

		bool updated = false;

		while (curr_file != files.end() && record.path >= *curr_file)
		{
			if (record.path == *curr_file)
			{
				// UPDATE_INDEX_MODIFY: output the new record
				// UPDATE_INDEX_ADD: throw exception
				// UPDATE_INDEX_REMOVE: skip the record
				if (flags & UPDATE_INDEX_MODIFY)
					output << IndexRecord(create_blob_from_file(*curr_file), *curr_file) << L"\n";
				else if (flags & UPDATE_INDEX_ADD)
					throw FileAlreadyInIndexException(*curr_file);
				else if ((flags & UPDATE_INDEX_FORCE_REMOVE) != UPDATE_INDEX_FORCE_REMOVE && std::filesystem::exists(record.path))
					throw IndexRemoveFileExists(*curr_file);
				updated = true;
			}
			else if (*curr_file < record.path)
			{
				if (flags & UPDATE_INDEX_ADD)
					output << IndexRecord(create_blob_from_file(*curr_file), *curr_file) << L"\n";
				else
					throw FileNotInIndexException(*curr_file);
			}
			curr_file++;
		}

		if (!updated)
			output << record << L"\n";
	}

	// finish reading the entire file
	while (!(stream >> record).eof())
		output << record << L"\n";

	if (curr_file != files.end())
	{
		if (flags & UPDATE_INDEX_ADD)
		{
			while (curr_file != files.end())
			{
				output << IndexRecord(create_blob_from_file(*curr_file), *curr_file) << L"\n";
				curr_file++;
			}
		}
		else
			throw FileNotInIndexException(*curr_file);
	}

	stream.close();
	Filesystem::write_content(Config::IndexFile, (std::wistream&)output, Filesystem::FILE_FLAG_OVERWRITE);
}