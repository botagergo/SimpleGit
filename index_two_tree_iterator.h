#pragma once

#include <fstream>

#include "index.h"
#include "tree.h"

class IndexTwoTreeIterator
{
public:
	IndexTwoTreeIterator(std::wifstream& index_in_stream, std::wifstream& tree1_in_stream, std::wifstream& tree2_in_stream)
		: _index_in_stream(index_in_stream), _tree1_in_stream(tree1_in_stream), _tree2_in_stream(tree2_in_stream)
	{
		_end = false;
		_first_iter = true;
		next();
	}

	void next();
	bool end() const { return _end; };

	const IndexRecord& index() const { return _index; };
	const TreeRecord& tree1() const { return _tree1; };
	const TreeRecord& tree2() const { return _tree2; };

	bool hasIndex() const { return _has_index; };
	bool hasTree1() const { return _has_tree1; };
	bool hasTree2() const { return _has_tree2; };

private:
	IndexRecord	_index;
	TreeRecord	_tree1;
	TreeRecord	_tree2;

	bool		_has_index;
	bool		_has_tree1;
	bool		_has_tree2;

	std::wifstream& _index_in_stream;
	std::wifstream& _tree1_in_stream;
	std::wifstream& _tree2_in_stream;

	bool		_end;
	bool		_first_iter;
};