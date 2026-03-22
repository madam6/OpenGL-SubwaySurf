#pragma once
#include "Entity.h"
class EntityParser
{
public:
	static std::unique_ptr<Entity> Create(const std::vector<std::string>& lines);
};