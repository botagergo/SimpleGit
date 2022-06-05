#pragma once

#include <sstream>

#include "defs.h"

#include "object.h"

class Repository;

class BlobReader : public ObjectReader
{
	friend class Object;

public:
	virtual std::ostream&		pretty_print(std::ostream& out_stream) override;
	std::vector<std::string>	read_lines();
	void						operator>>(std::string& str);

protected:
	BlobReader(const ObjectData& data, const Object& blob) : ObjectReader(data, blob) {}
};

std::string			write_blob(const Repository& repo, const std::string& content);
std::string			write_blob_from_file(const Repository& repo, const fs::path& filename);

// reads a blob into the working directory
void				read_blob(const Repository& repo, const std::string& id, const fs::path& file);
