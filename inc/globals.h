#pragma once

#include <map>
#include <string>

#include <boost/filesystem/path.hpp>

#include "defs.h"

namespace Globals
{
	extern fs::path			DefaultSimpleGitDir;
	extern fs::path			DefaultSimpleGitConfig;

	extern fs::path			SimpleGitDir;
	extern fs::path			ObjectDir;
	extern fs::path			TagDir;
	extern fs::path			RefDir;
	extern fs::path			BranchDir;

	extern fs::path			IndexFile;
	extern fs::path			ConfigFile;
	extern fs::path			GlobalConfigFile;
	extern fs::path			HeadFile;

	extern fs::path			CommitMessageTmpFile;
	extern fs::path			ObjectTmpFile;

	extern std::map<std::string, std::string>	Config;

	extern const size_t		IdPrefixLength;
	extern const size_t		MaxObjectHeaderSize;

	extern bool				Debug;
	extern bool				Verbose;

	extern fs::path			ExecutableDir;
	extern fs::path			CommitMessageTemplateFile;
	extern std::string		CommitMessagePromptString;

	extern std::string		EditorCommand;
};