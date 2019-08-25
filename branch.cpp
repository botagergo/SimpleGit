#include <filesystem>

#include "branch.h"
#include "exception.h"
#include "filesystem.h"
#include "globals.h"
#include "helper.h"

void set_branch(const std::wstring& name, const std::wstring& object_id, bool overwrite)
{
	try {
		Filesystem::write_content(Globals::BranchDir / name, object_id, overwrite);
	} catch (FileExistsException) {
		throw BranchExistsException(name);
	}
}
