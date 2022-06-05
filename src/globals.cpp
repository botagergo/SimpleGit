#include <map>

#include <boost/dll.hpp>

#include "globals.h"
#include "helper.h"

namespace Globals
{
	const size_t		IdPrefixLength = 2;
	const size_t		MaxObjectHeaderSize = 32;

	bool				Quiet = false;
	bool				Debug = false;
	bool				Verbose = false;

	fs::path			ExecutableDir = boost::dll::program_location().parent_path();
	std::string			EditorCommand = get_git_editor();
};
