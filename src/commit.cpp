#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/process.hpp>

#include "branch.h"
#include "commit.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"
#include "object.h"
#include "repository.h"

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

std::string write_commit(const Repository& repo, const Commit& commit)
{
	ObjectWriter writer(repo, "commit");

	writer << "tree " << commit.tree_id << '\n';
	for (const std::string& parent : commit.parents)
		writer << "parent " << parent << '\n';
	writer << "author " << commit.author << '\n';
	writer << "committer " << commit.committer << "\n\n";
	writer << commit.message;

	return writer.save();
}

std::string get_commit_message(const Repository& repo, const std::string& init)
{
	Filesystem::write_content(repo.commitMessageTmpFile, init + "\n" + Repository::commitMessagePromptString, Filesystem::FILE_FLAG_OVERWRITE);

	std::ostringstream cmd;
	cmd << Globals::EditorCommand  << " " << repo.commitMessageTmpFile;

	if (boost::process::system(cmd.str().c_str()) != 0)
		throw Exception(boost::format("error executing command: %1%") % cmd.str());

	std::vector<std::string> lines = Filesystem::read_lines(repo.commitMessageTmpFile);
	std::ostringstream msg;

	for (std::string line : lines)
	{
		boost::algorithm::trim(line);
		if (!line.empty() && line[0] != '#')
			msg << line << '\n';
	}

	std::string ret = msg.str();
	boost::algorithm::trim(ret);
	return ret;
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
