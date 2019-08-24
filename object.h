#pragma once

struct ObjectHeader
{
	std::wstring object_kind;
};

ObjectHeader parse_object_header(std::wistream& stream);