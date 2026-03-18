#pragma once

#include "Common.h"
#include "Camera.h"

class CameraMode
{
public:
	CameraMode() = default;
	virtual ~CameraMode() = default;

	virtual camData GetData(double dt, const camData& currentData) = 0;
};

