#include "index.h"
#include "index_two_tree_iterator.h"

void IndexTwoTreeIterator::next()
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
