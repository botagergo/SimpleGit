#include <iostream>

#include <boost/program_options.hpp>

#include "config.h"
#include "error.h"
#include "filesystem.h"
#include "globals.h"
#include "repository.h"

namespace po = boost::program_options;

Repository Repository::create(const fs::path& git_dir, bool bare)
{
	Repository repo;

	const char* env;

	repo.gitDir = git_dir;
	repo.repositoryConfigFile = repo.gitDir / "config";
	repo.userConfigFile = get_home_directory() / ".sgitconfig";
	repo.systemConfigFile = "/etc/sgitconfig";

	if (env = std::getenv("GIT_OBJECT_DIRECTORY"))
		repo.objectDir = env;
	else
		repo.objectDir = repo.gitDir / repo.objectDirName;

	repo.refDir = repo.gitDir / repo.refDirName;
	repo.tagDir = repo.refDir / repo.tagDirName;
	repo.branchDir = repo.refDir / repo.branchDirName;

	repo.indexFile = repo.gitDir / repo.indexFileName;
	repo.headFile = repo.gitDir / repo.headFileName;

	repo.commitMessageTmpFile = repo.gitDir / "COMMIT_EDITMSG";
	repo.objectTmpFile = repo.gitDir / "OBJECT";

	repo.bare = bare;;

	return repo;
}

Repository Repository::create()
{
	Repository repo;

	fs::path git_dir;

	const char* git_dir_env = std::getenv("GIT_DIR");
	if (git_dir_env)
	{
		git_dir = git_dir_env;
		if (!is_git_dir(git_dir))
    	{
			throw Exception(boost::format("not a git repository: ")
        		% git_dir);
		}
	}
	else
	{
		git_dir = find_git_dir(fs::current_path());
		if (git_dir == fs::path())
		{
			throw Exception(boost::format("not a git repository (or any of the parent directories): ")
        		% git_dir);
		}
	}

	return create(git_dir);
}

fs::path Repository::get_git_dir()
{
	const char* gitdir = std::getenv("GIT_DIR");
	return gitdir ? gitdir : Repository::defaultGitDirName;
}

fs::path Repository::find_git_dir(const fs::path& origin)
{
    fs::path curr_dir = origin, ret_dir;
    for (;;)
    {
        if (fs::exists(ret_dir = curr_dir / defaultGitDirName) && is_git_dir(ret_dir))
            return ret_dir;
    }
}

bool Repository::is_git_dir(const fs::path& path)
{
    return fs::exists(path / headFileName)
        && fs::exists(path / objectDirName)
        && fs::exists(path / refDirName);
}

void Repository::init_config_file(const fs::path& cfg_file)
{
	Config config = Config::create({cfg_file});
	config.set(repositoryConfigFile, "core.bare", bare);
}

Repository::InitType Repository::init(const fs::path& template_dir)
{
	bool reinitialize = is_git_dir(gitDir);

    try
    {
        Filesystem::copy_directory(template_dir, gitDir,
            Filesystem::FILE_FLAG_SKIP_EXISTING | Filesystem::FILE_FLAG_RECURSIVE);
    }
    catch (const std::exception& e)
    {
        warning(boost::format("failed to copy template directory: %1%") % e.what());
    }

    if (!reinitialize)
    {
        // TODO: create HEAD
        Filesystem::write_content(headFile, "ref: master");
        Filesystem::write_content(repositoryConfigFile, "", Filesystem::FILE_FLAG_OVERWRITE);
	    Filesystem::create_directory(objectDir);
	    Filesystem::create_directory(refDir);
	    Filesystem::create_directory(tagDir);
	    Filesystem::create_directory(branchDir);

        init_config_file(repositoryConfigFile);
    }

	return reinitialize ? InitType::REINIT : INIT;
}

void Repository::write_git_dir_ref(const fs::path& git_dir, const fs::path& ref_file)
{
    Filesystem::write_content(ref_file, (boost::format("gitdir: %1%") % git_dir).str());
}

const fs::path		Repository::defaultGitDirName = ".sgit";
const fs::path		Repository::headFileName = "HEAD";
const fs::path		Repository::indexFileName = "INDEX";
const fs::path		Repository::objectDirName = "objects";
const fs::path		Repository::refDirName = "refs";
const fs::path		Repository::tagDirName = "tags";
const fs::path		Repository::branchDirName = "heads";

const fs::path		Repository::gitCoreDir = "/usr/share/sgit-core/";
const fs::path		Repository::defaultTemplateDir = gitCoreDir / "templates";

const fs::path		Repository::commitMessageTemplateFile = "/usr/share/sgit/COMMIT_EDITMSG_template";
const std::string	Repository::commitMessagePromptString = "# Please enter the commit message for your changes. Lines starting \
# with '#' will be ignored, and an empty message aborts the commit.";
