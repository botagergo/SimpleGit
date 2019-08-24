#pragma once

// if ref is a tag or branch, sets "referenced" and returns true
// if ref is an object id, returns false
// else throws InvalidRefException
bool get_referenced(const std::wstring &ref, std::wstring &referenced);
void get_referenced_object(const std::wstring &ref, std::wstring &referenced);