#include "filesystem.h"
#include "globals.h"
#include "repository.h"

#include <iostream>

fs::path get_git_dir()
{
	const char* gitdir = std::getenv("GIT_DIR");
	return gitdir ? gitdir : Globals::DefaultGitDirName;
}

fs::path find_git_dir(const fs::path& origin)
{
    fs::path curr_dir = origin, ret_dir;
    for (;;)
    {
        if (fs::exists(ret_dir = curr_dir / Globals::DefaultGitDirName) && is_git_dir(ret_dir))
            return ret_dir;
    }
}

bool is_git_dir(const fs::path& path)
{
    return fs::exists(path / Globals::HeadFileName)
        && fs::exists(path / Globals::ObjectDirName)
        && fs::exists(path / Globals::RefDirName);
}

void init_git_dir(const fs::path& template_dir)
{
    Filesystem::copy_directory(template_dir, Globals::GitDir,
        Filesystem::FILE_FLAG_OVERWRITE | Filesystem::FILE_FLAG_RECURSIVE);

    // TODO: create HEAD
    Filesystem::write_content(Globals::ConfigFile, "", Filesystem::FILE_FLAG_OVERWRITE);
	Filesystem::create_directory(Globals::ObjectDir);
	Filesystem::create_directory(Globals::RefDir);
	Filesystem::create_directory(Globals::TagDir);
	Filesystem::create_directory(Globals::BranchDir);
}