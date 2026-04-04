#pragma once
#include "Common.h"
#include "Component.h"
struct RenderData;
class EntityParser;
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

		ref.SetOwner(this);

		m_Components.emplace_back(std::move(comp));
		return ref;
	}

	template<typename T>
	T* FindComponent()
	{
		for (auto& component : m_Components)
		{
			T* ptr = dynamic_cast<T*>(component.get());
			if (ptr != nullptr)
			{
				return ptr;
			}
		}
		return nullptr;
	}

	friend class EntityParser;
private:
	std::vector<std::unique_ptr<Component>> m_Components;
	std::string m_Name;
};