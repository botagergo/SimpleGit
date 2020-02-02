#pragma once

template <typename Tree1InStream, typename Tree2InStream>
class TwoTreeIterator
{
public:
	TwoTreeIterator(Tree1InStream& _tree1_in_stream, Tree2InStream& _tree2_in_stream) : _tree1_in_stream(_tree1_in_stream), _tree2_in_stream(_tree2_in_stream)
	{
		_first_iter = true;
		_end = false;
		next();
	}

	void next();
	bool end() const { return _end; };

	const TreeRecord& tree1() const { return _tree1; };
	const TreeRecord& tree2() const { return _tree2; };

	bool hasTree1() const { return _has_tree1; };
	bool hasTree2() const { return _has_tree2; };

private:
	TreeRecord	_tree1;
	TreeRecord	_tree2;

	Tree1InStream& _tree1_in_stream;
	Tree2InStream& _tree2_in_stream;

	bool		_first_iter;
	bool		_has_tree1;
	bool		_has_tree2;
	bool		_end;
};

template<typename Tree1InStream, typename Tree2InStream>
inline void TwoTreeIterator<Tree1InStream, Tree2InStream>::next()
{
	if (_first_iter)
	{
		_tree1_in_stream >> _tree1;
		_tree2_in_stream >> _tree2;
		_first_iter = false;
	}
	else
	{
		if (_has_tree1)
			_tree1_in_stream >> _tree1;
		if (_has_tree2)
			_tree2_in_stream >> _tree2;
	}

	_has_tree1 = _has_tree2 = false;

	if (_tree1_in_stream && _tree2_in_stream)
	{
		if (_tree1.path <= _tree2.path)
		{
			_has_tree1 = true;
			if (_tree1.path == _tree2.path)
				_has_tree2 = true;
		}
	}

	else if (_tree1_in_stream)
		_has_tree1 = true;
	else if (_tree2_in_stream)
		_has_tree2 = true;
	else
		_end = true;
}
