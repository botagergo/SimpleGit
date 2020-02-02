#pragma once

#include <fstream>

#include "defs.h"
#include "index.h"
#include "tree.h"

template <typename IndexInStream, typename Tree1InStream, typename Tree2InStream>
class IndexTwoTreeIterator
{
public:
	IndexTwoTreeIterator(IndexInStream& index_in_stream, Tree1InStream& tree1_in_stream, Tree2InStream& tree2_in_stream)
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

	IndexInStream& _index_in_stream;
	Tree1InStream& _tree1_in_stream;
	Tree2InStream& _tree2_in_stream;

	bool		_end;
	bool		_first_iter;
};

template <typename IndexInStream, typename Tree1InStream, typename Tree2InStream>
void IndexTwoTreeIterator<IndexInStream, Tree1InStream, Tree2InStream>::next()
{
	if (_first_iter)
	{
		_index_in_stream >> _index;
		_tree1_in_stream >> _tree1;
		_tree2_in_stream >> _tree2;
		_first_iter = false;
	}
	else
	{
		if (_has_index)
			_index_in_stream >> _index;
		if (_has_tree1)
			_tree1_in_stream >> _tree1;
		if (_has_tree2)
			_tree2_in_stream >> _tree2;
	}

	_has_index = _has_tree1 = _has_tree2 = false;

	if (_index_in_stream && _tree1_in_stream && _tree2_in_stream)
	{
		if (_index.path <= _tree1.path && _index.path <= _tree2.path)
		{
			_has_index = true;
			if (_index.path == _tree1.path)
			{
				_has_tree1 = true;
				if (_index.path == _tree2.path)
					_has_tree2 = true;
			}
		}
		else if (_tree1.path <= _index.path && _tree1.path <= _tree2.path)
		{
			_has_tree1 = true;
			if (_tree1.path == _index.path)
			{
				_has_index = true;
				if (_tree1.path == _tree2.path)
					_has_tree2 = true;
			}
		}
		else
		{
			_has_tree2 = true;
			if (_tree2.path == _tree1.path)
			{
				_has_tree1 = true;
				if (_tree2.path == _index.path)
					_has_index = true;
			}
		}
	}
	else if (_index_in_stream && _tree1_in_stream)
	{
		if (_index.path <= _tree1.path)
		{
			_has_index = true;
			if (_index.path == _tree1.path)
				_has_tree1 = true;
		}
		else
			_has_tree1 = true;
	}
	else if (_index_in_stream && _tree2_in_stream)
	{
		if (_index.path <= _tree2.path)
		{
			_has_index = true;
			if (_index.path == _tree2.path)
				_has_tree2 = true;
		}
		else
			_has_tree2 = true;
	}
	else if (_tree1_in_stream && _tree2_in_stream)
	{
		if (_tree1.path <= _tree2.path)
		{
			_has_tree1 = true;
			if (_tree1.path == _tree2.path)
				_has_tree2 = true;
		}
		else
			_has_tree2 = true;
	}
	else if (_index_in_stream)
		_has_index = true;
	else if (_tree1_in_stream)
		_has_tree1 = true;
	else if (_tree2_in_stream)
		_has_tree2 = true;
	else
		_end = true;
}