#pragma once

#include "defs.h"
#include "object.h"

class BlobReader : public ObjectReader
{
	friend class Object;

public:
	virtual std::ostream& pretty_print(std::ostream& out_stream) override
	{
		out_stream << _curr_pos;
		return out_stream;
	}

	void operator>>(std::string& str)
	{
		str = _curr_pos;
	}

protected:
	BlobReader(const ObjectData& data) : ObjectReader(data) {}
};

std::string			write_blob(const std::string& content);
std::string			write_blob_from_file(const fs::path& filename);

// reads a blob into the working directory
void				read_blob(const std::string& id, const fs::path& file);
