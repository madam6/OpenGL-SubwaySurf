#pragma once
#include "Cameras/CameraMode.h"
#include "ECS/Entity.h"
#include <memory>

class TopDownCam : public CameraMode
{
public:
	TopDownCam();
	camData GetData(double dt, const camData& currentData) override;
private:
	std::weak_ptr<Entity> m_TargetEntity;
};
