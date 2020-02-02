#pragma once

#include "dtl/dtl.hpp"

class IndexRecord;

class Diff
{
public:
	Diff(const std::vector<std::string>& lines1, const std::vector<std::string>& lines2)
		: d(lines1, lines2) {};

	long long calculate()
	{
		d.compose();
		return d.getEditDistance();
	}

	void print()
	{
		d.composeUnifiedHunks();
		d.printUnifiedFormat();
	}

private:
	dtl::Diff<std::string, std::vector<std::string>> d;
};

// returns true if the path in the record doesn't exist 
// or the content of the path is different from the object in the record
bool		has_diff(const IndexRecord& record);

void		diff_tree(const std::string& tree1_id, const std::string& tree2_id);
void		diff_index(const fs::path& index_file, const std::vector<fs::path>& files);
