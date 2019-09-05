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

Commit read_commit(const std::string& commit_id)
{
	std::ifstream in_stream;
	open_commit(in_stream, commit_id);

	fs::path commit_file = Filesystem::get_object_path(commit_id);

	try {
		return read_commit(in_stream);
	}
	catch (CommitFileCorrupted) {
		throw CommitFileCorrupted(commit_file);
	}
}

Commit read_commit(std::istream& in_stream)
{
	Commit commit;
	std::string str;

	in_stream >> str;
	if (str != "tree")
		throw CommitFileCorrupted("");
	in_stream >> commit.tree_id;

	in_stream >> str;
	if (str != "parents")
		throw CommitFileCorrupted("");
	std::getline(in_stream, str);
	std::stringstream ss(str);
	while (ss >> str)
		commit.parents.push_back(str);

	in_stream >> str >> std::ws;
	if (str != "author")
		throw CommitFileCorrupted("");
	std::getline(in_stream, str);
	commit.author = str;

	in_stream >> str >> std::ws;
	if (str != "committer")
		throw CommitFileCorrupted("");
	in_stream >> commit.committer;

	in_stream >> std::ws;

	commit.message = Filesystem::read_content(in_stream);

	return commit;
}

std::string write_commit(const Commit& commit)
{
	ObjectFileWriter writer;
	writer << "commit" << "\n";
	writer << "tree" << ' ' << commit.tree_id << "\n";
	writer << "parents";
	for (const std::string& parent : commit.parents)
		writer << ' ' << parent;
	writer << "\n";
	writer << "author" << ' ' << commit.author << "\n";
	writer << "committer" << ' ' << commit.committer << "\n";
	writer << commit.message;
	return writer.save();
}

std::string resolve_ref(const std::string& ref)
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

void open_commit(std::ifstream& in_stream, const std::string& commit_id)
{
	std::string object_type = open_object(in_stream, commit_id);
	if (object_type != "commit")
		throw NotCommitException(commit_id);
}