#pragma once

#include <filesystem>
#include <map>
#include <string>

#include <boost/filesystem.hpp>

struct Tag;
namespace fs = boost::filesystem;

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
	extern fs::path			HeadFile;

	extern size_t							IdPrefixLength;

	extern std::map<std::string, std::string>	Config;
};