#pragma once

#include "object.h"

struct Commit
{
	Commit() {}

	std::string					tree_id;
	std::vector<std::string>	parents;
	std::string					author;
	std::string					committer;
	std::string					message;
};

class CommitReader : public ObjectReader
{
	friend class Object;

public:
	virtual std::ostream& pretty_print(std::ostream& out_stream) override;

	CommitReader& operator>>(Commit& commit);

protected:
	CommitReader(const ObjectData& data) : ObjectReader(data) {}
};

std::string		write_commit(const Commit& commit);

std::ostream&	operator<<(std::ostream& out_stream, const Commit& commit);