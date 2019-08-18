#pragma once

#include "object.h"

class Ref
{
public:
	virtual Object resolve() = 0;
	virtual Object referenced() = 0;
};