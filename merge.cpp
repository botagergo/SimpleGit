#include <fstream>

#include "exception.h"
#include "filesystem.h"
#include "index.h"
#include "merge.h"
#include "object_file_writer.h"
#include "tree.h"

void merge_tree_into_index(const fs::path& index_file, const std::wstring& tree_id)
{
	std::wofstream out_stream;
	std::wifstream index_in, tree_in;

	fs::path tmp_index_file = index_file.wstring() + L"_TMP";

	out_stream.open(tmp_index_file.wstring());
	Filesystem::open(index_file, (std::wfstream&)index_in, std::ios_base::in);
	if (Filesystem::open_object(tree_id, tree_in) != L"tree")
		throw NotTreeException(tree_id);

	TreeRecord index_record, tree_record;
	index_in >> index_record;
	tree_in >> tree_record;

	while (index_in && tree_in)
	{
		if (index_record.path < tree_record.path)
		{
			out_stream << IndexRecord(index_record.id, index_record.path) << L'\n';
			index_in >> index_record;
		}
		else if (tree_record.path < index_record.path)
		{
			out_stream << tree_record << L'\n';
			tree_in >> index_record;
		}
		else if (index_record.id == tree_record.id)
		{
			out_stream << index_record << L'\n';
			tree_in >> tree_record;
		}
		else if (index_record.kind == L"blob")
		{
			throw MergeConflictException();
		}
		else /*if (tree1_record.kind == L"tree")*/
		{
			std::wstring new_tree_id = merge_tree(index_record.id, tree_record.id);
			out_stream << TreeRecord(L"tree", new_tree_id, index_record.path) << L'\n';
		}
	}

	while (index_in)
	{
		out_stream << index_record << L'\n';
		tree_in >> index_record;
	}

	while (tree_in)
	{
		out_stream << tree_record << L'\n';
		tree_in >> tree_record;
	}

	out_stream.close();
	fs::rename(tmp_index_file, index_file);
}



std::wstring merge_tree(const std::wstring& tree1_id, const std::wstring& tree2_id)
{
	ObjectFileWriter new_tree;
	std::wifstream tree1_in, tree2_in;

	if (Filesystem::open_object(tree1_id, tree1_in) != L"tree")
		throw NotTreeException(tree1_id);
	if (Filesystem::open_object(tree2_id, tree2_in) != L"tree")
		throw NotTreeException(tree2_id);

	TreeRecord tree1_record, tree2_record;
	tree1_in >> tree1_record;
	tree2_in >> tree2_record;

	while (tree1_in && tree2_in)
	{
		if (tree1_record.path < tree2_record.path)
		{
			new_tree << tree1_record << L'\n';
			tree1_in >> tree1_record;
		}
		else if (tree2_record.path < tree1_record.path)
		{
			new_tree << tree2_record << L'\n';
			tree2_in >> tree1_record;
		}
		else if (tree1_record.id == tree2_record.id)
		{
			new_tree << tree1_record << L'\n';
			tree2_in >> tree2_record;
		}
		else if (tree1_record.kind == L"blob")
		{
			throw MergeConflictException();
		}
		else /*if (tree1_record.kind == L"tree")*/
		{
			std::wstring new_tree_id = merge_tree(tree1_record.id, tree2_record.id);
			new_tree << TreeRecord(L"tree", new_tree_id, tree1_record.path) << L'\n';
		}
	}

	while (tree1_in)
	{
		new_tree << tree1_record << L'\n';
		tree1_in >> tree1_record;
	}

	while (tree2_in)
	{
		new_tree << tree2_record << L'\n';
		tree2_in >> tree2_record;
	}

	return new_tree.save();
}
