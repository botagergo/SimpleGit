#include <filesystem>

#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "ref.h"
#include "tag.h"

bool get_referenced(const std::string &ref, std::string &referenced)
{
	if (fs::exists(Globals::TagDir / ref))
	{
		referenced = Filesystem::read_content(Globals::TagDir / ref);
		return true;
	}
	else if (fs::exists(Globals::BranchDir / ref))
	{
		referenced = Filesystem::read_content(Globals::BranchDir / ref);
		return true;
	}
	else if (fs::exists(Filesystem::get_object_path(ref)))
		return false;
	else
		throw InvalidReferenceException(ref);
}

void get_referenced_object(const std::string &ref, std::string &referenced)
{
	std::string ret;
	while (get_referenced(ref, ret))
		;
	referenced = ret;
}
