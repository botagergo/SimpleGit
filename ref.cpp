#include <filesystem>

#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "ref.h"
#include "tag.h"

bool get_referenced(const std::wstring &ref, std::wstring &referenced)
{
	if (std::filesystem::exists(Config::TagDir / ref))
	{
		referenced = Filesystem::read_content(Config::TagDir / ref);
		return true;
	}
	else if (std::filesystem::exists(Config::BranchDir / ref))
	{
		referenced = Filesystem::read_content(Config::BranchDir / ref);
		return true;
	}
	else if (std::filesystem::exists(Filesystem::get_object_path(ref)))
		return false;
	else
		throw InvalidReferenceException(ref);
}

void get_referenced_object(const std::wstring &ref, std::wstring &referenced)
{
	std::wstring ret;
	while (get_referenced(ref, ret))
		;
	referenced = ret;
}
