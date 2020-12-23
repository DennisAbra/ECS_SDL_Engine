#pragma once
#include "ECSSettings.h"
#include <set>
#include "Position.h"

class System
{
public:
	std::set<Entity> entities;
};