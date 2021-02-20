#include <fstream>
#include <codecvt>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/process.hpp>

#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "platform.h"
#include "sha1.h"
#include <zlib.h>

std::string get_hash(const char* text, size_t size)
{
	char* buf = new char[size];
	memcpy(buf, text, size);

	SHA1_CTX ctx;
	SHA1Init(&ctx);
	unsigned char hash[20];

	SHA1Update(&ctx, (const unsigned char*)buf, (u_int32_t)size);

	SHA1Final(hash, &ctx);
	std::string ret;
	for (int i = 0; i < 20; i++)
	{
		unsigned char ch = hash[i];
		ret += int_to_hex_char(ch >> 4);
		ret += int_to_hex_char(ch & 15);
	}

	delete buf;
	return ret;
}

std::string get_file_hash(const fs::path &file)
{
	std::string str = Filesystem::read_content(file);
	return get_hash(str.c_str(), str.size());
}

char int_to_hex_char(unsigned a)
{
	switch (a)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'a';
	case 11: return 'b';
	case 12: return 'c';
	case 13: return 'd';
	case 14: return 'e';
	case 15: return 'f';
	default: assert(false); return '?';
	}
}

char hex_char_to_byte(char ch)
{
	switch (ch)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'a': return 10;
	case 'b': return 11;
	case 'c': return 12;
	case 'd': return 13;
	case 'e': return 14;
	case 'f': return 15;
	default: assert(false); return 0;
	}
}

time_t get_time_t_now()
{
	time_t now;
	time(&now);
	return now;
}

std::string get_git_editor()
{
	const char* dir = nullptr;
	for (const char* varname : { "GIT_EDITOR", "VISUAL", "EDITOR" })
	{
		dir = std::getenv(varname);
		if (dir)
			return dir;
	}

	return get_default_git_editor();
}

std::string get_commit_message(const std::string& init)
{
	Filesystem::write_content(Globals::CommitMessageTmpFile, init + "\n" + Globals::CommitMessagePromptString, Filesystem::FILE_FLAG_OVERWRITE);

	std::ostringstream cmd;
	cmd << Globals::EditorCommand  << " " << Globals::CommitMessageTmpFile;

	if (boost::process::system(cmd.str().c_str()) != 0)
		throw Exception(boost::format("error executing command: %1%") % cmd.str());

	std::vector<std::string> lines = Filesystem::read_lines(Globals::CommitMessageTmpFile);
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

void check_error_code(const boost::system::error_code& ec, const std::string& arg)
{
	if (ec)
	{
		std::string msg = ec.message();
		if (!arg.empty())
			msg += ": " + arg;
		throw Exception(msg);
	}
}

void check_error_code(const std::error_code& ec, const std::string& arg)
{
	if (ec)
	{
		std::string msg = ec.message();
		if (!arg.empty())
			msg += ": " + arg;
		throw Exception(msg);
	}
}