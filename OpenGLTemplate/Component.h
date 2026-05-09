#pragma once
#include "Common.h"
#include "Game.h"

// Basic part of the entity component system, composed onto entities through entities.cfg
// Mast register itself in ComponentRegistry with ComponentRegistry::Instance().Register to be applied correctly
class Component
{
public:
    virtual ~Component() = default;
    // Needs to be overriden to apply properties of the component. 
    // Parser passes in value after semi column as a string. Its up to you how to interpret that and assign to member variables
    virtual void Apply(const PropertyMap& props) {};
    virtual void Init() {}
    // Executed every frame
    virtual void Update(float dt) {}
    // Submit RenderData to the queue for Renderer to process
    virtual void AddRenderData(std::vector<RenderData>&) {}
    void SetOwner(Entity* owner) { m_pOwner = owner; }
    // Allows to query parent Entity. Can be used to search neighbouring components
    Entity* GetOwner() const { return m_pOwner; }
protected:
    Entity* m_pOwner = nullptr;
};