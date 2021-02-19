#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "branch.h"
#include "commit.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "object.h"

std::ostream&	operator<<(std::ostream& out_stream, const UserInfo& user)
{
	out_stream << user.name << " <" << user.email << '>';
	return out_stream;
}

std::istream&	operator>>(std::istream& in_stream, UserInfo& user)
{
	in_stream >> user.name;
	
	while(in_stream && in_stream.get() != '<')
		;

	if (!in_stream)
		throw Exception("format error");

	std::stringstream email;
	char ch;
	while (in_stream && (ch = in_stream.get()) != '>')
		email << ch;

	if (!in_stream)
		throw Exception("format error");

	user.email = email.str();

	return in_stream;
}

std::ostream& CommitReader::pretty_print(std::ostream& out_stream)
{
	Commit commit;
	*this >> commit;
	out_stream << commit;
	return out_stream;
}

CommitReader& CommitReader::operator>>(Commit& commit)
{
	std::istringstream ss(_curr_pos);
	std::string line;

	while (std::getline(ss, line))
	{
		std::istringstream line_ss(line);
		std::string record_type;
		line_ss >> record_type;

		if (!line_ss)
			break;

		if (record_type == "tree")
			line_ss >> commit.tree_id;
		else if (record_type == "parent")
		{
			std::string parent;
			line_ss >> parent;
			commit.parents.push_back(parent);
		}
		else if (record_type == "author")
			line_ss >> commit.author;
		else if (record_type == "committer")
			line_ss >> commit.committer;
	}

	commit.message = Filesystem::read_content(ss);

	return *this;
}

Commit CommitReader::read_commit()
{
	try
	{
		Commit commit;
		*this >> commit;
		return commit;
	}
	catch (const Exception&)
	{
		throw Exception(boost::format("commit file corrupted: %1%") % object().path().c_str());
	}
}

std::string write_commit(const Commit& commit)
{
	ObjectWriter writer("commit");

	writer << "tree " << commit.tree_id << '\n';
	for (const std::string& parent : commit.parents)
		writer << "parent " << parent << '\n';
	writer << "author " << commit.author << '\n';
	writer << "committer " << commit.committer << "\n\n";
	writer << commit.message;

	return writer.save();
}

std::ostream& operator<< (std::ostream& out_stream, const Commit& commit)
{
	out_stream << "tree" << "\t\t" << commit.tree_id << '\n';
	for (const std::string& parent : commit.parents)
		out_stream << "parent" << "\t\t" << parent << '\n';
	out_stream << "author" << "\t\t" << commit.author << '\n';
	out_stream << "committer" << '\t' << commit.committer << '\n';
	out_stream << commit.message;
	return out_stream;
}