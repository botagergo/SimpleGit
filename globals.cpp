#include <filesystem>

#include "globals.h"

namespace Config
{
	std::filesystem::path			SimpleGitDir;
	std::filesystem::path			ObjectDir;
	std::filesystem::path			RefDir;
	std::filesystem::path			TagDir;
	std::filesystem::path			BranchDir;

	std::filesystem::path			IndexFile;

	size_t							IdPrefixLength = 2;

	std::vector<Tag*>				Tags;
	std::wstring					Head;
};