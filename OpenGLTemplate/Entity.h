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
	std::shared_ptr<T> FindComponent()
	{
		for (auto& component : m_Components)
		{
			if (auto ptr = std::dynamic_pointer_cast<T>(component))
			{
				return ptr;
			}
		}
		return nullptr;
	}
	std::string GetName() const { return m_Name; };
	void SetName(const std::string& name) { m_Name = name; };
	friend class EntityParser;
private:
	std::vector<std::shared_ptr<Component>> m_Components;
	std::string m_Name;
};