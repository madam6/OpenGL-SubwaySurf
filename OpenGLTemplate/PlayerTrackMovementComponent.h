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
	void StartRecovery();
	void ApplySpeedBoost(float milliSeconds);
	bool IsRecovering() const { return m_IsRecovering; }
	float GetCurrentDistance() const { return m_CurrentDistance; }
	bool IsSpeedBoostActive() const { return m_IsSpeedBoostActive; }
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
	
	float m_RecordTimer = 0.0f;
	float m_RecordedDistance = 0.0f;
	int m_RecordedLane = 0;
	float m_RecordedLaneOffset = 0.0f;

	bool m_IsRecovering = false;
	bool m_IsSpeedBoostActive = false;
	float m_SpeedBoostTimer = 0.0f;
	float m_SpeedBoostTimeOut = 0.0f;
	float m_SpeedBoost = 0.1f;

	float m_RecoveryTimer = 0.0f;
	float m_RecoveryDuration = 1.f;
	float m_StartRecoveryDistance = 0.0f;
	float m_StartRecoveryLaneOffset = 0.0f;

	// properties
	float m_Speed{};
	float m_HeightFactor{};
};