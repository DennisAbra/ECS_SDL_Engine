#pragma once
#include <bitset>

using Entity = std::uint16_t;
const Entity MAX_ENTITIES = 5000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;
using ComponentList = std::bitset<MAX_COMPONENTS>;