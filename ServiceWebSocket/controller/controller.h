#pragma once
#include "../mysql/DBPool.h"
class controller
{
public:
	static bool registerUser(std::string name, int passwd, std::string nameStr = "", std::string mail = "");
};

