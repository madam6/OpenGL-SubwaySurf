#pragma once
#include "Common.h"
#include "Component.h"
struct RenderData;
class Entity
{
public:
	Entity() = default;
	~Entity() = default;

	void Init();
	void Update(float dt);
	void AddRenderData(std::vector<RenderData>& renderQueue);

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		auto comp = std::make_unique<T>(std::forward<Args>(args)...);
		T& ref = *comp;
		m_Components.emplace_back(std::move(comp));
		return ref;
	}
private:
	std::vector<std::unique_ptr<Component>> m_Components;
};