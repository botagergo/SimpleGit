#pragma once

#include <iostream>
#include <string>

inline void error(const std::wstring &msg)
{
	std::wcout << L"error: " << msg << std::endl;
	exit(1);
}

inline void message(const std::wstring &msg)
{
	std::wcout << msg << std::endl;
}