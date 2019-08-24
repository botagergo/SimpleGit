#pragma once

#include <string>
#include <vector>

void cmd_init(const std::vector<std::wstring>& args);
void cmd_add(const std::vector<std::wstring>& args);
void cmd_update_index(const std::vector<std::wstring>& args);
void cmd_commit(const std::vector<std::wstring>& args);
void cmd_tag(const std::vector<std::wstring>& args);
void cmd_branch(const std::vector<std::wstring>& args);
void cmd_stash(const std::vector<std::wstring>& args);
void cmd_checkout(const std::vector<std::wstring>& args);
void cmd_merge(const std::vector<std::wstring>& args);