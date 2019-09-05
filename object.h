#pragma once

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "filesystem.h"
#include "zlib/zlib.h"

namespace fs = boost::filesystem;

struct ObjectHeader
{
	const static long long MaxHeaderSize = 32;
	std::string kind;
	long long	size;
};

void uncompress_file(Bytef* dest, const char* filename, uLongf num_bytes_to_uncompress)
{
	HANDLE file = CreateFileA(filename, GENERIC_READ, 0, NULL, 0, 0, 0);
	HANDLE map = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, num_bytes_to_uncompress, NULL);
	void* ptr = MapViewOfFile(map, FILE_MAP_READ, 0, 0, num_bytes_to_uncompress);

	uLongf dest_len;
	uncompress(dest, &dest_len, (Bytef*)ptr, num_bytes_to_uncompress);
	dest[dest_len] = 0;

	UnmapViewOfFile(ptr);
	CloseHandle(file);
}

ObjectHeader	read_object_header(const std::string& object_id)
{
	const fs::path filename = Filesystem::get_object_path(object_id);

	char* buf = new char[ObjectHeader::MaxHeaderSize];
	uncompress_file((Bytef*)buf, filename.string().c_str(), )
}

bool			match_object_id(const std::string& prefix, const std::string& object_id);

std::string		expand_object_id_prefix(const std::string& object_id_prefix);
		
std::string		open_object(std::ifstream& in_stream, const std::string& object_id);
		
std::string		get_object_type(const std::string& object_id);

void			pretty_print_object(std::ostream& out_stream, const std::string& object_id);