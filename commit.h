#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "object.h"

struct Commit
{
	Commit() {}

	std::string				tree_id;
	std::vector<std::string>	parents;
	std::string				author;
	std::string				committer;
	std::string				message;
};

// reads a commit
Commit			read_commit(const std::string& commit_id);
Commit			read_commit(std::istream& in_stream);

// creates a commit with the given attributes
std::string write_commit(const Commit& commit);

std::string resolve_ref(const std::string& ref);

void		open_commit(std::ifstream& in_stream, const std::string& commit_id);

template <typename OutStream, typename InStream>
void pretty_print_commit(OutStream& out_stream, InStream& in_stream)
{
	Commit commit = read_commit(in_stream);

	out_stream << "tree" << "\t\t" << commit.tree_id << '\n';
	for (const std::string& parent : commit.parents)
		out_stream << "parent" << "\t\t" << parent << '\n';
	out_stream << "author" << "\t\t" << commit.author << '\n';
	out_stream << "committer" << '\t' << commit.committer << '\n';
	out_stream << commit.message;
}
