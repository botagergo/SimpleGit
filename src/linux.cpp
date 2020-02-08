#include "platform.h"

#ifdef LINUX_
#include <pwd.h>
#include <unistd.h>
#include "fcntl.h"
#include "errno.h"
#include "sys/mman.h"
#include "sys/stat.h"

#include <boost/filesystem/path.hpp>

#include "defs.h"
#include "exception.h"

std::string get_username()
{
	uid_t uid = geteuid();
	struct passwd* pw = getpwuid(uid);
	if (pw)
		return std::string(pw->pw_name);
	else
		return {};
}

std::string get_default_git_editor()
{
	return "vi";
}

fs::path get_home_directory()
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	return homedir;
}

namespace Filesystem
{
	void set_hidden(const fs::path& path)
	{
	}
}

uint64_t create_file_map_read(const char* object_file, void** ptr)
{
	int fd = open(object_file, O_RDONLY);
	if (fd == -1)
		throw Exception(strerror(errno));

	struct stat statbuf;
	stat(object_file, &statbuf);
	uint64_t fsize = statbuf.st_size;

	*ptr = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
		throw Exception(strerror(errno));

	close(fd);
	return fsize;
}

void create_file_map_write(const char* object_file, void** ptr, uint64_t fsize)
{
	int fd = open(object_file, O_CREAT | O_RDWR, 0600);
	if (fd == -1)
		throw Exception(strerror(errno));

	fallocate(fd, 0, 0, fsize);

	void* addr = mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
		throw Exception("mmap error");

	close(fd);
	return fsize;
}

void free_file_map(void* ptr, uint64_t size)
{
	munmap(ptr, size);
}
#endif