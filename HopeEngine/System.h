#pragma once
#include "ECSSettings.h"
#include <set>

class System
{
public:
	std::set<Entity> entities;
};