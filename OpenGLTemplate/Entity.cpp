#include "Entity.h"
#include "Component.h"

void Entity::Init()
{
	for (auto& component : m_Components)
	{
		component->Init();
	}
}

void Entity::Update(float dt)
{
	for (auto& component : m_Components)
	{
		component->Update(dt);
	}
}

void Entity::AddRenderData(std::vector<RenderData>& renderQueue)
{
	for (auto& component : m_Components)
	{
		component->AddRenderData(renderQueue);
	}
}