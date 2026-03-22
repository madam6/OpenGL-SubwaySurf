#pragma once
#include "Common.h"
#include "Game.h"

class Component
{
public:
    virtual ~Component() = default;
    virtual void Init() {}
    virtual void Update(float dt) {}
    virtual void AddRenderData(std::vector<RenderData>&) {}
};