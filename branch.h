#pragma once

#include <string>

#include "id.h"

class Branch
{
public:
	std::wstring	description;
	Id				*commit;
};