#pragma once
#include "Component.h"
#include "Common.h"
#include "EventSystem.h"
class ModelViewComponent;
class CatmullRomComponent;
class PlayerTrackMovementComponent : public Component, public IObserver
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Update(float dt) override;

	void OnEvent(const std::string& eventName, const EventData& data) override;
public:
	void Apply(const PropertyMap& props);
	~PlayerTrackMovementComponent()
	{
		EventSystem::Instance().Unsubscribe("KeyDown", this);
	}
private:
	std::shared_ptr<ModelViewComponent> m_ModelViewComponentRef;
	std::shared_ptr<CatmullRomComponent> m_CatmullRomComponentRef;
	float m_CurrentDistance = 0.0f;

	int m_TargetLane = 0;
	float m_CurrentLaneOffset = 0.0f;
	float m_LaneWidth = 10.0f;

	// properties
	float m_Speed{};
	float m_HeightFactor{};
};