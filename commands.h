#pragma once

#include <string>
#include <vector>

void cmd_init(int argc, wchar_t* argv[]);
void cmd_add(int argc, wchar_t* argv[]);
void cmd_update_index(int argc, wchar_t* argv[]);
void cmd_commit(int argc, wchar_t* argv[]);
void cmd_tag(int argc, wchar_t* argv[]);
void cmd_read_tree(int argc, wchar_t* argv[]);
void cmd_write_tree(int argc, wchar_t* argv[]);
void cmd_branch(int argc, wchar_t* argv[]);
void cmd_stash(int argc, wchar_t* argv[]);
void cmd_checkout(int argc, wchar_t* argv[]);
void cmd_merge(int argc, wchar_t* argv[]);
void cmd_cat_file(int argc, wchar_t* argv[]);