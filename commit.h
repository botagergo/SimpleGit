#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "object.h"

struct Commit
{
	Commit() {}

	std::wstring				tree_id;
	std::vector<std::wstring>	parents;
	std::wstring				author;
	std::wstring				committer;
	std::wstring				message;
};

// reads a commit
Commit			read_commit(const std::wstring& commit_id);
Commit			read_commit(std::wistream& in_stream);

// creates a commit with the given attributes
std::wstring write_commit(const Commit& commit);

std::wstring resolve_ref(const std::wstring& ref);

void		open_commit(std::wifstream& in_stream, const std::wstring& commit_id);

void		pretty_print_commit(std::wostream& out_stream, std::wistream& in_stream);