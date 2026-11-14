#pragma once
#include "ECS/Entity.h"
class EntityParser
{
public:
	static std::shared_ptr<Entity> Create(const std::vector<std::string>& lines);
};