#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "object.h"

std::wstring create_commit(const std::wstring &tree_id, const std::vector<std::wstring> &parents, const std::wstring &author, const std::wstring &committer, const std::wstring &message);