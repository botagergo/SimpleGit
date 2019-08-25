#include <fstream>
#include <windows.h>
#include <filesystem>

#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "sha1.h"

std::wstring to_wide_string(const std::string& str)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);
}

std::string to_string(const std::wstring &str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
}

std::wstring get_hash(const std::wstring &text)
{
	// std::string str = to_string(text); ////////////////////////////
	return to_wide_string(sha1(to_string(text)));
}

std::wstring get_file_hash(const fs::path &file)
{
	return get_hash(Filesystem::read_content(file));
}