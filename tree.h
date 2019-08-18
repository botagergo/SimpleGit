#pragma once

#include <vector>

#include "base.h"
#include "object.h"

class Tree : public Object
{
public:
	std::vector<ObjectId> children;
};