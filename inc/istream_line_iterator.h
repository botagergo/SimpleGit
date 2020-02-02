#pragma once

#include <iterator>

class istream_line_iterator
{
public:
	typedef std::wstring value_type;

	istream_line_iterator(std::istream &istream);

	istream_line_iterator operator++(int);
	istream_line_iterator operator++();

	value_type operator*();

	bool end() const;

	bool operator==(const istream_line_iterator& other);
	bool operator!=(const istream_line_iterator& other);
};