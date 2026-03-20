#pragma once
#include "CameraMode.h"
#include <fstream>

class PathBuilderCam : public CameraMode
{
public:
	PathBuilderCam();
	~PathBuilderCam();
	camData GetData(double dt, const camData& currentData) override;
private:
	// Rotate the camera viewpoint -- this effectively rotates the camera
	void RotateViewPoint(float angle, const glm::vec3 viewPoint);

	// Respond to mouse movement to rotate the camera
	void SetViewByMouse();
	void RecordPoint();



	camData m_camData;

	std::ofstream m_File;


	float m_TimePeriod;
	float m_ElapsedTime;
	float m_Speed = 0.05f;
};