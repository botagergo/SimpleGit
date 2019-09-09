#pragma once

#include <fstream>

#include "defs.h"
#include "index.h"
#include "tree.h"

class IndexTwoTreeIterator
{
public:
	IndexTwoTreeIterator(std::ifstream& index_in_stream, std::ifstream& tree1_in_stream, std::ifstream& tree2_in_stream)
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

	std::ifstream& _index_in_stream;
	std::ifstream& _tree1_in_stream;
	std::ifstream& _tree2_in_stream;

	bool		_end;
	bool		_first_iter;
};