#include <map>

#include <boost/dll.hpp>

#include "globals.h"
#include "helper.h"

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

	fs::path			RepositoryConfigFile;
	fs::path			UserConfigFile = get_home_directory() / ".sgitconfig";
	fs::path			SystemConfigFile = "/etc/sgitconfig";

	fs::path			GitDir;

	fs::path			ObjectDir;
	fs::path			RefDir;
	fs::path			TagDir;
	fs::path			BranchDir;

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

	void init()
	{
		if (Globals::GitDir == fs::path())
		{
			assert(false); // Globals::GitDir has to be initialized before running init_path_constants()!
		}

		Globals::RepositoryConfigFile = Globals::GitDir / "config";

		Globals::ObjectDir = Globals::GitDir / Globals::ObjectDirName;
		Globals::RefDir = Globals::GitDir / Globals::RefDirName;
		Globals::TagDir = Globals::RefDir / Globals::TagDirName;
		Globals::BranchDir = Globals::RefDir / Globals::BranchDirName;

		Globals::IndexFile = Globals::GitDir / Globals::IndexFileName;
		Globals::HeadFile = Globals::GitDir / Globals::HeadFileName;

		Globals::CommitMessageTmpFile = Globals::GitDir / "COMMIT_EDITMSG";
		Globals::ObjectTmpFile = Globals::GitDir / "OBJECT";
		Globals::ExecutableDir = boost::dll::program_location().parent_path();
		Globals::CommitMessageTemplateFile = "/usr/share/sgit/COMMIT_EDITMSG_template";

		Globals::EditorCommand = get_git_editor();
	}
};