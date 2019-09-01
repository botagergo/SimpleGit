#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>

inline void error(const wchar_t* fmt...)
{
	wprintf(L"error: ");
	wprintf(fmt);
	exit(1);
}

inline void error(const char* fmt...)
{
	printf("error: ");
	printf(fmt);
	exit(1);
}

inline void error(const std::wstring &msg)
{
	std::wcout << L"error: " << msg << L'\n';
	exit(1);
}

inline void message(const std::wstring &msg)
{
	std::wcout << msg << L'\n';
}

inline void message(const boost::wformat& msg)
{
	message(msg.str());
}

inline void message(const wchar_t* fmt...)
{
	wprintf(fmt);
}