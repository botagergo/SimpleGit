#pragma once

#include "object.h"

class Blob : public Object
{
public:
	std::wstring path;
	std::wstring content;
};