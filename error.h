#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>

inline void error(const char* fmt...)
{
	printf("error: ");
	printf(fmt);
	exit(1);
}

inline void error(const std::string &msg)
{
	std::cout << "error: " << msg << '\n';
	exit(1);
}

inline void message(const char* fmt...)
{
	printf(fmt);
}

inline void message(const std::string& msg)
{
	message(msg.c_str());
}

inline void message(const boost::format& msg)
{
	message(msg.str());
}