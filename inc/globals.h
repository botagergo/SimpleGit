#pragma once

#include <map>
#include <string>

#include <boost/filesystem/path.hpp>

#include "defs.h"

namespace Globals
{
	extern const size_t		IdPrefixLength;
	extern const size_t		MaxObjectHeaderSize;

	extern bool				Quiet;
	extern bool				Debug;
	extern bool				Verbose;

	extern fs::path			ExecutableDir;
	extern std::string		EditorCommand;
};
