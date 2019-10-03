#include <algorithm>
#include <fstream>
#include <string>

#include "dtl/dtl/dtl.hpp"

#include "blob.h"
#include "diff.h"
#include "index.h"
#include "object.h"
#include "tree.h"
#include "two_tree_iterator.h"

void diff_tree(const std::string& tree1_id, const std::string& tree2_id)
{
	auto tree1Reader = Object(tree1_id).get_tree_reader();
	auto tree2Reader = Object(tree2_id).get_tree_reader();

	TwoTreeIterator iter(*tree1Reader, *tree2Reader);
	for (; !iter.end(); iter.next())
	{
		if (iter.hasTree1() && iter.hasTree2())
		{
			if (iter.tree1().kind == "blob" && iter.tree2().kind == "blob")
			{
				Diff d(Object(iter.tree1().id).get_blob_reader()->read_lines(),
					Object(iter.tree2().id).get_blob_reader()->read_lines());

				if (d.calculate())
				{
					std::cout << "a/" << iter.tree1().path.string() << " b/" << iter.tree2().path.string() << '\n';
					d.print();
				}
			}
			else if (iter.tree1().kind == "tree" && iter.tree2().kind == "tree")
				diff_tree(iter.tree1().id, iter.tree2().id);
			else
				assert(false); // TODO
		}
		else if (iter.hasTree1())
		{
			std::cout << "file removed: " << iter.tree1().path.string() << '\n';
		}
		else if (iter.hasTree2())
		{
			std::cout << "file added: " << iter.tree2().path.string() << '\n';
		}
		else
			assert(false);
		std::cout << '\n';
	}
}

void diff_index(const fs::path& index_file, const std::vector<fs::path>& files)
{
	std::ifstream index_in_stream;
	Filesystem::open(index_file, index_in_stream);

	IndexRecord record;
	while (index_in_stream >> record)
	{
		if (files.empty() || std::find(files.begin(), files.end(), record.path) != files.end())
		{
			if (!fs::exists(record.path))
			{
				std::cout << "deleted file: " << record.path.string() << '\n';
				std::cout << "--- a/" << record.path.string() << "\n+++ /dev/null\n";
			}
			else if (Filesystem::get_stat(record.path).st_mtime != record.mtime)
			{
				Diff d(Object(record.id).get_blob_reader()->read_lines(),
					Filesystem::read_lines(record.path));

				if (d.calculate())
				{
					std::cout << "--- a/" << record.path.string() << "\n+++ b/" << record.path.string() << '\n';
					d.print();
					std::cout << '\n';
				}
			}
		}
	}
}