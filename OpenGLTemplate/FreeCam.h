#pragma once
#include "CameraMode.h"

class FreeCam : public CameraMode
{
public:
	FreeCam();

	camData GetData(double dt, const camData& currentData) override;
private:
	// Rotate the camera viewpoint -- this effectively rotates the camera
	void RotateViewPoint(float angle, const glm::vec3 viewPoint);

	// Respond to mouse movement to rotate the camera
	void SetViewByMouse();
	camData m_camData;
};