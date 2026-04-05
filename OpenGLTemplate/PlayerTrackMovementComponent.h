#pragma once
#include "Component.h"
#include "Common.h"
class ModelViewComponent;
class CatmullRomComponent;
class PlayerTrackMovementComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Update(float dt) override;
public:
	void Apply(const PropertyMap& props);
private:
	std::shared_ptr<ModelViewComponent> m_ModelViewComponentRef;
	std::shared_ptr<CatmullRomComponent> m_CatmullRomComponentRef;
	float m_CurrentDistance = 0.0f;
	// properties
	float m_Speed{};
};