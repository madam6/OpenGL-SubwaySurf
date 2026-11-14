#pragma once
#include "Cameras/CameraMode.h"
#include "ECS/Entity.h"
#include <memory>

class FollowCam : public CameraMode
{
public:
	FollowCam();
	camData GetData(double dt, const camData& currentData) override;
private:
	std::weak_ptr<Entity> m_TargetEntity;
};