#pragma once
#include "Cameras/CameraMode.h"
#include "ECS/Entity.h"
#include <memory>

class FirstPersonCam : public CameraMode
{
public:
	FirstPersonCam();
	camData GetData(double dt, const camData& currentData) override;
private:
	std::weak_ptr<Entity> m_TargetEntity;
};