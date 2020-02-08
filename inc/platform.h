#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define WINDOWS_
#else
	#define LINUX_
#endif

#include <string>

#include "defs.h"

