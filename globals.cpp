#include <map>

#include "globals.h"

namespace Globals
{
	fs::path			DefaultSimpleGitDir;
	fs::path			DefaultSimpleGitConfig;

	fs::path			SimpleGitDir;
	fs::path			ObjectDir;
	fs::path			RefDir;
	fs::path			TagDir;
	fs::path			BranchDir;

	fs::path			ConfigFile;
	fs::path			IndexFile;
	fs::path			HeadFile;

	std::string			Head;

	std::map<std::string, std::string>	Config;

	const size_t		IdPrefixLength = 2;
	const size_t		MaxObjectHeaderSize = 32;
};