#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include "windows.h"
#else
	#include "linux.h"
#endif
