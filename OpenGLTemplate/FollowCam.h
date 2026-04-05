#pragma once
#include "CameraMode.h"
#include "Entity.h"
#include <memory>

class FollowCam : public CameraMode
{
public:
	FollowCam();
	camData GetData(double dt, const camData& currentData) override;
private:
	std::shared_ptr<Entity> m_TargetEntity;
};