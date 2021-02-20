#include <map>

#include "globals.h"

namespace Globals
{
	fs::path			DefaultGitDirName = ".sgit";
	
	fs::path			HeadFileName = "HEAD";
	fs::path			IndexFileName = "INDEX";
	fs::path			ObjectDirName = "objects";
	fs::path			RefDirName = "refs";
	fs::path			TagDirName = "tags";
	fs::path			BranchDirName = "heads";

	fs::path			GitCoreDir = "/usr/share/sgit-core/";
	fs::path			DefaultTemplateDir = Globals::GitCoreDir / "templates";

	fs::path			DefaultSimpleGitConfig;

	fs::path			GitDir;

	fs::path			ObjectDir;
	fs::path			RefDir;
	fs::path			TagDir;
	fs::path			BranchDir;

	fs::path			ConfigFile;
	fs::path			GlobalConfigFile;
	fs::path			IndexFile;
	fs::path			HeadFile;

	fs::path			CommitMessageTmpFile;
	fs::path			ObjectTmpFile;

	std::string			Head;

	std::map<std::string, std::string>	Config;

	const size_t		IdPrefixLength = 2;
	const size_t		MaxObjectHeaderSize = 32;

	bool				Debug = false;
	bool				Verbose = false;

	fs::path			ExecutableDir;
	fs::path			CommitMessageTemplateFile;
	std::string			CommitMessagePromptString = "# Please enter the commit message for your changes. Lines starting \
							# with '#' will be ignored, and an empty message aborts the commit.";

	std::string			EditorCommand;
};