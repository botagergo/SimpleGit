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