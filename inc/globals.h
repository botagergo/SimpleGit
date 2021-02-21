#pragma once

#include <map>
#include <string>

#include <boost/filesystem/path.hpp>

#include "defs.h"

namespace Globals
{
	extern fs::path			DefaultGitDirName;

	extern fs::path			HeadFileName;
	extern fs::path			IndexFileName;
	extern fs::path			ObjectDirName;
	extern fs::path			RefDirName;
	extern fs::path			TagDirName;
	extern fs::path			BranchDirName;

	extern fs::path			GitCoreDir;
	extern fs::path			DefaultTemplateDir;

	extern fs::path			RepositoryConfigFile;
	extern fs::path			UserConfigFile;
	extern fs::path			SystemConfigFile;

	extern fs::path			GitDir;

	extern fs::path			ObjectDir;
	extern fs::path			TagDir;
	extern fs::path			RefDir;
	extern fs::path			BranchDir;

	extern fs::path			IndexFile;
	extern fs::path			HeadFile;

	extern fs::path			CommitMessageTmpFile;
	extern fs::path			ObjectTmpFile;

	extern std::map<std::string, std::string>	Config;

	extern const size_t		IdPrefixLength;
	extern const size_t		MaxObjectHeaderSize;

	extern bool				Quiet;
	extern bool				Debug;
	extern bool				Verbose;

	extern fs::path			ExecutableDir;
	extern fs::path			CommitMessageTemplateFile;
	extern std::string		CommitMessagePromptString;

	extern std::string		EditorCommand;

	void init();
};