#pragma once
#include "CameraMode.h"
#include "Entity.h"
#include <memory>

class SideCamera : public CameraMode
{
public:
	SideCamera();
	camData GetData(double dt, const camData& currentData) override;
private:
	std::weak_ptr<Entity> m_TargetEntity;
};