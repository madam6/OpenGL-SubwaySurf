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
    void SetOwner(Entity* owner) { m_pOwner = owner; }
    Entity* GetOwner() const { return m_pOwner; }
protected:
    Entity* m_pOwner = nullptr;
};