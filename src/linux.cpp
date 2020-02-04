#include <boost/filesystem/path.hpp>

#include "defs.h"
#include "linux.h"

std::string get_username()
{
	uid_t uid = geteuid();
	struct passwd* pw = getpwuid(uid);
	if (pw)
		return std::string(pw->pw_name);
	else
		return {};
}

std::string get_default_git_editor()
{
	return "vi";
}

fs::path get_home_directory()
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	return homedir;
}