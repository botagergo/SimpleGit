#pragma once

#include <chrono>
#include <string>

#include "object.h"
#include "tree.h"

class Commit : public Object
{
	std::wstring	author;
	std::wstring	commiter;
	std::wstring	message;
	//TODO: add date and time
	
	Commit			*parent;
	Tree			*tree;
};