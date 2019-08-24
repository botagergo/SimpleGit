#pragma once

#include <filesystem>
#include <string>

struct Tag;

namespace Config
{
	const std::filesystem::path			DefaultSimpleGitDir;

	extern std::filesystem::path		SimpleGitDir;
	extern std::filesystem::path		ObjectDir;
	extern std::filesystem::path		TagDir;
	extern std::filesystem::path		RefDir;
	extern std::filesystem::path		BranchDir;

	extern std::filesystem::path		IndexFile;

	extern size_t						IdPrefixLength;
};