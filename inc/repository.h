#pragma once

#include "defs.h"

fs::path    get_git_dir();
fs::path    find_git_dir(const fs::path& origin);
bool        is_git_dir(const fs::path& path);
void        init_git_dir(const fs::path& template_dir, bool reinitialize=false);
void        write_git_dir_ref(const fs::path& git_dir, const fs::path& ref_file);