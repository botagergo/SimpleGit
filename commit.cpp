#include <cassert>
#include <fstream>
#include <iostream>

#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "object.h"
#include "object_file_writer.h"

Commit read_commit(const std::wstring& commit_id)
{
	std::wfstream in_stream;
	std::wstring object_type = open_object(commit_id, in_stream);

	std::wstring commit_file = Filesystem::get_object_path(commit_id);

	if (object_type != L"commit")
		throw NotCommitException(commit_id);

	Commit commit;
	std::wstring str;

	in_stream >> str;
	if (str != L"tree")
		throw CommitFileCorrupted(commit_file);
	in_stream >> commit.tree_id;

	in_stream >> str;
	if (str != L"parents")
		throw CommitFileCorrupted(commit_file);
	std::getline(in_stream, str);
	std::wstringstream ss(str);
	while (ss >> str)
		commit.parents.push_back(str);

	in_stream >> str >> std::ws;
	if (str != L"author")
		throw CommitFileCorrupted(commit_file);
	std::getline(in_stream, str);
	commit.author = str;

	in_stream >> str >> std::ws;
	if (str != L"committer")
		throw CommitFileCorrupted(commit_file);
	in_stream >> commit.committer;

	commit.message = Filesystem::read_content(in_stream);

	return commit;
}

std::wstring write_commit(	const std::wstring&				 tree_id,
							const std::vector<std::wstring>& parents,
							const std::wstring&				 author,
							const std::wstring&				 committer,
							const std::wstring&				 message)
{
	ObjectFileWriter writer;
	writer << L"commit" << L"\n";
	writer << L"tree" << L' ' << tree_id << L"\n";
	writer << L"parents";
	for (const std::wstring& parent : parents)
		writer << L' ' << parent;
	writer << L"\n";
	writer << L"author" << L' ' << author << L"\n";
	writer << L"committer" << L' ' << committer << L"\n";
	writer << message;
	return writer.save();
}
