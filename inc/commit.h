#pragma once

#include "object.h"

struct UserInfo
{
	UserInfo(const std::string& name, const std::string& email)
		: name(name), email(email) {}
	UserInfo() {}

	std::string name;
	std::string email;
};

std::ostream&	operator<<(std::ostream& out_stream, const UserInfo& user);

struct Commit
{
	Commit() {}

	std::string								tree_id;
	std::vector<std::string>				parents;
	UserInfo								author;
	UserInfo								committer;
	std::string								message;
};

class CommitReader : public ObjectReader
{
	friend class Object;

public:
	virtual std::ostream&	pretty_print(std::ostream& out_stream) override;
	Commit					read_commit();

	CommitReader& operator>>(Commit& commit);

protected:
	CommitReader(const ObjectData& data, const Object& object) : ObjectReader(data, object) {}
};

std::string		write_commit(const Commit& commit);

std::ostream&	operator<<(std::ostream& out_stream, const Commit& commit);