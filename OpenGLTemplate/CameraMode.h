#pragma once

#include "Common.h"
#include "Camera.h"

// Startegy for Camera.h
class CameraMode
{
public:
	CameraMode() = default;
	virtual ~CameraMode() = default;

	// Main function to be defined by deriving strategies
	// Should tell how modify camData based on previous value
	virtual camData GetData(double dt, const camData& currentData) = 0;
};

