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

// creates a commit with the given attributes
std::wstring	write_commit(const std::wstring				 &tree_id,
							 const std::vector<std::wstring> &parents,
							 const std::wstring				 &author,
							 const std::wstring				 &committer,
							 const std::wstring				 &message);