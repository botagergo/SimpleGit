#include <cassert>
#include <fstream>
#include <iostream>

#include "branch.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "object.h"
#include "object_file_writer.h"
#include "tag.h"

Commit read_commit(const std::wstring& commit_id)
{
	std::wifstream in_stream;
	open_commit(in_stream, commit_id);

	fs::path commit_file = Filesystem::get_object_path(commit_id);

	try {
		return read_commit(in_stream);
	}
	catch (CommitFileCorrupted) {
		throw CommitFileCorrupted(commit_file);
	}
}

Commit read_commit(std::wistream& in_stream)
{
	Commit commit;
	std::wstring str;

	in_stream >> str;
	if (str != L"tree")
		throw CommitFileCorrupted(L"");
	in_stream >> commit.tree_id;

	in_stream >> str;
	if (str != L"parents")
		throw CommitFileCorrupted(L"");
	std::getline(in_stream, str);
	std::wstringstream ss(str);
	while (ss >> str)
		commit.parents.push_back(str);

	in_stream >> str >> std::ws;
	if (str != L"author")
		throw CommitFileCorrupted(L"");
	std::getline(in_stream, str);
	commit.author = str;

	in_stream >> str >> std::ws;
	if (str != L"committer")
		throw CommitFileCorrupted(L"");
	in_stream >> commit.committer;

	in_stream >> std::ws;

	commit.message = Filesystem::read_content(in_stream);

	return commit;
}

std::wstring write_commit(const Commit& commit)
{
	ObjectFileWriter writer;
	writer << L"commit" << L"\n";
	writer << L"tree" << L' ' << commit.tree_id << L"\n";
	writer << L"parents";
	for (const std::wstring& parent : commit.parents)
		writer << L' ' << parent;
	writer << L"\n";
	writer << L"author" << L' ' << commit.author << L"\n";
	writer << L"committer" << L' ' << commit.committer << L"\n";
	writer << commit.message;
	return writer.save();
}

std::wstring resolve_ref(const std::wstring& ref)
{
	// TODO: resolve tags
	try {
		if (is_tag(ref))
			return resolve_tag(ref);
		if (is_branch(ref))
			return resolve_branch(ref);
		else if (ref.size() >= 4)
			return expand_object_id_prefix(ref);
		else
			throw ResolveRefException(ref);
	}
	catch (const std::exception& e) {
		throw ResolveRefException(ref);
	}
}

void open_commit(std::wifstream& in_stream, const std::wstring& commit_id)
{
	std::wstring object_type = open_object(in_stream, commit_id);
	if (object_type != L"commit")
		throw NotCommitException(commit_id);
}

void pretty_print_commit(std::wostream& out_stream, std::wistream& in_stream)
{
	Commit commit = read_commit(in_stream);

	out_stream << L"tree" << L"\t\t" << commit.tree_id << L'\n';
	for (const std::wstring& parent : commit.parents)
		out_stream << L"parent" << L"\t\t" << parent << L'\n';
	out_stream << L"author" << L"\t\t" << commit.author << L'\n';
	out_stream << L"committer" << L'\t' << commit.committer << L'\n';
	out_stream << commit.message;
}
