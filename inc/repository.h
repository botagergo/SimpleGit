#pragma once

#include "defs.h"

class Repository
{
public:
	enum InitType { INIT, REINIT };

	static Repository				create(const fs::path& git_dir, bool bare=false);
	static Repository				create();

	InitType						init(const fs::path& template_dir);
	void							init_config_file(const fs::path& cfg_file);

	static void						write_git_dir_ref(const fs::path& git_dir, const fs::path& ref_file);

	const static fs::path			defaultGitDirName;

	const static fs::path			headFileName;
	const static fs::path			indexFileName;
	const static fs::path			objectDirName;
	const static fs::path			refDirName;
	const static fs::path			tagDirName;
	const static fs::path			branchDirName;

	const static fs::path			gitCoreDir;
	const static fs::path			defaultTemplateDir;

	const static fs::path			commitMessageTemplateFile;
	const static std::string		commitMessagePromptString;

	fs::path						repositoryConfigFile;
	fs::path						userConfigFile;
	fs::path						systemConfigFile;

	fs::path						gitDir;

	fs::path						objectDir;
	fs::path						tagDir;
	fs::path						refDir;
	fs::path						branchDir;

	fs::path						indexFile;
	fs::path						headFile;

	fs::path						commitMessageTmpFile;
	fs::path						objectTmpFile;

	bool							bare;

	std::map<std::string, std::string>	config;

private:
	static fs::path get_git_dir();
	static fs::path find_git_dir(const fs::path& origin);
	static bool is_git_dir(const fs::path& path);
};
