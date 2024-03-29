#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>

#include "defs.h"

#define FATAL std::cerr << "fatal: "
#define WARNING std::cerr << "warning: "
#define MESSAGE \
		if (Globals::Quiet) {} \
    	else std::cout

inline void error(const char* fmt...)
{
	printf("error: ");
	printf(fmt);
	printf("\n");
	exit(1);
}

inline void error(const std::string &msg)
{
	std::cout << "error: " << msg << '\n';
	exit(1);
}

inline void fatal(const char* fmt...)
{
	printf("fatal: ");
	printf(fmt);
	printf("\n");
	exit(1);
}

inline void fatal(const std::string &msg)
{
	fatal(msg.c_str());
}

inline void fatal(const boost::format& msg)
{
	fatal(msg.str());
}

inline void warning(const char* fmt...)
{
	printf("warning: ");
	printf(fmt);
	printf("\n");
}

inline void warning(const std::string &msg)
{
	warning(msg.c_str());
}

inline void warning(const boost::format& msg)
{
	warning(msg.str());
}
inline void message(const char* fmt...)
{
	printf(fmt);
	exit(0);
}

inline void message(const std::string& msg)
{
	message(msg.c_str());
}

inline void message(const boost::format& msg)
{
	message(msg.str());
}

template <typename T>
inline void message(const T& msg)
{
	message(std::to_string(msg));
}