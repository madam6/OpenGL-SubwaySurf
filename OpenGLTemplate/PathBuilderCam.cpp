#include "PathBuilderCam.h"
#include "gamewindow.h"
#include "FreeCam.h"
#include "Common.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"

PathBuilderCam::PathBuilderCam()
	: m_TimePeriod(100.f)
	, m_ElapsedTime(0.f)
{
	m_camData.position = glm::vec3();
	m_camData.upVector = glm::vec3(0.f, 1.f, 0.f);
	m_camData.viewpoint = glm::vec3(0.f, 0.f, -1.f);
	m_File.open("C:\\Temp\\track.csv", std::ios::out | std::ios::trunc);
	if (!m_File.is_open()) {
		OutputDebugStringA("Failed to open track.csv!\n");
	}
	else {
		OutputDebugStringA("track.csv is open and ready to write!\n");
	}
}

PathBuilderCam::~PathBuilderCam()
{
	m_File.close();
}

camData PathBuilderCam::GetData(double dt, const camData& currentData)
{
	m_camData = currentData;

	SetViewByMouse();

	glm::vec3 forward = glm::normalize(m_camData.viewpoint - m_camData.position);

	glm::vec3 right = glm::normalize(glm::cross(forward, m_camData.upVector));

	m_camData.upVector = glm::normalize(glm::cross(right, forward));

	m_camData.position += forward * (float)(m_Speed * dt);
	m_camData.viewpoint = m_camData.position + forward;

	m_ElapsedTime += dt;

	if (m_ElapsedTime >= m_TimePeriod)
	{
		m_ElapsedTime = 0.f;
		RecordPoint();
	}

	return m_camData;
}

void PathBuilderCam::SetViewByMouse()
{
	int middle_x = GameWindow::SCREEN_WIDTH >> 1;
	int middle_y = GameWindow::SCREEN_HEIGHT >> 1;

	POINT mouse;
	GetCursorPos(&mouse);

	if (mouse.x == middle_x && mouse.y == middle_y) return;
	if (abs(mouse.x - middle_x) < 1 && abs(mouse.y - middle_y) < 1) return;

	SetCursorPos(middle_x, middle_y);

	float angle_y = (float)(middle_x - mouse.x) / 1000.0f;
	float angle_z = (float)(middle_y - mouse.y) / 1000.0f;

	glm::vec3 forward = glm::normalize(m_camData.viewpoint - m_camData.position);
	glm::vec3 right = glm::normalize(glm::cross(forward, m_camData.upVector));

	if (glm::length(right) > 0.0001f)
	{
		RotateViewPoint(angle_z, right);

		glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), angle_z, right);
		m_camData.upVector = glm::normalize(glm::vec3(pitchMat * glm::vec4(m_camData.upVector, 0.0f)));
	}

	RotateViewPoint(angle_y, m_camData.upVector);
}

void PathBuilderCam::RecordPoint()
{
	OutputDebugString("Recording path....\n");
	m_File << m_camData.position.x << ',' << m_camData.position.y << ',' << m_camData.position.z << ','
		<< m_camData.upVector.x << ',' << m_camData.upVector.y << ',' << m_camData.upVector.z << std::endl;
}

void PathBuilderCam::RotateViewPoint(float fAngle, const glm::vec3 vAxis)
{
	glm::vec3 forward = glm::normalize(m_camData.viewpoint - m_camData.position);

	glm::mat4 R = glm::rotate(glm::mat4(1), fAngle, vAxis);

	forward = glm::normalize(glm::vec3(R * glm::vec4(forward, 0.0f)));

	m_camData.viewpoint = m_camData.position + forward;
}