#pragma once

bool			match_object_id(const std::wstring& prefix, const std::wstring& object_id);

std::wstring	expand_object_id_prefix(const std::wstring& object_id_prefix);

std::wstring	open_object(std::wifstream& in_stream, const std::wstring& object_id);

std::wstring	get_object_type(const std::wstring& object_id);

void			pretty_print_object(std::wostream& out_stream, const std::wstring& object_id);