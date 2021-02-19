#include "globals.h"
#include "repository.h"

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