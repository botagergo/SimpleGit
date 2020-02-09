#include "exception.h"
#include "platform.h"

#ifdef WINDOWS_
#include <string>

#include <windows.h>
#include <Lmcons.h>

#include "windows.h"

std::string get_username()
{
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameA(username, &username_len);
	return username;
}

std::string get_default_git_editor()
{
	return "notepad";
}

fs::path    get_home_directory()
{
	return getenv("USERPROFILE");
}
namespace Filesystem
{
	void set_hidden(const fs::path& path)
	{
		SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}
}

uint64_t create_file_map_read(const char* object_file, void** ptr, std::function<void(void)>& free_map)
{
	HANDLE file = CreateFileA(object_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (file == INVALID_HANDLE_VALUE)
		throw Exception(boost::format("cannot open file: %1%") % object_file);

	HANDLE map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (map == NULL)
		throw Exception("CreateFileMapping");

	*ptr = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
	if (ptr == NULL)
		throw Exception("MapViewOfFile");

	LARGE_INTEGER l_fsize;
	GetFileSizeEx(file, (PLARGE_INTEGER)& l_fsize);
	uint64_t fsize = l_fsize.QuadPart;

	free_map = [file, ptr]()
	{
		CloseHandle(file);
		UnmapViewOfFile(ptr);
	}

	return fsize;
}

void create_file_map_write(const char* object_file, void** ptr, uint64_t fsize, std::function<void(void)>& free_map)
{
	HANDLE file = CreateFileA(object_file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	if (file == NULL)
		throw Exception(boost::format("cannot create file: %1%") % object_file);

	HANDLE map = CreateFileMappingA(file, NULL, PAGE_READWRITE, 0, (DWORD)fsize, NULL);
	if (map == NULL)
		throw Exception("CreateFileMapping");

	*ptr = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, fsize);
	if (ptr == NULL)
		throw Exception("MapViewOfFile");

	free_map = [file, ptr]()
	{
		CloseHandle(file);
		UnmapViewOfFile(ptr);
	}
}
void free_file_map(void* ptr, uint64_t size)
{
	UnmapViewOfFile(ptr);
}
#endif
