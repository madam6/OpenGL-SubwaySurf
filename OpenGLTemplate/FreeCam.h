#pragma once
#include "CameraMode.h"

class FreeCam : public CameraMode
{
public:
	FreeCam();

	camData GetData(double dt, const camData& currentData) override;
private:
	// Respond to mouse movement to rotate the camera
	void SetViewByMouse();
	camData m_camData;
};