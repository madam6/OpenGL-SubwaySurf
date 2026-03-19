#include "PathBuilderCam.h"
#include "gamewindow.h"
#include "FreeCam.h"
#include "Common.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"

PathBuilderCam::PathBuilderCam()
{
	m_camData.position = glm::vec3();
	m_camData.upVector = glm::vec3();
	m_camData.viewpoint = glm::vec3();
}

camData PathBuilderCam::GetData(double dt, const camData& currentData)
{
	m_camData = currentData;
	SetViewByMouse();
	return m_camData;
}

void PathBuilderCam::SetViewByMouse()
{
	int middle_x = GameWindow::SCREEN_WIDTH >> 1;
	int middle_y = GameWindow::SCREEN_HEIGHT >> 1;

	float angle_y = 0.0f;
	float angle_z = 0.0f;
	static float rotation_x = 0.0f;

	POINT mouse;
	GetCursorPos(&mouse);

	if (mouse.x == middle_x && mouse.y == middle_y) {
		return;
	}

	SetCursorPos(middle_x, middle_y);

	angle_y = (float)(middle_x - mouse.x) / 10000.0f;
	angle_z = (float)(middle_y - mouse.y) / 10000.0f;

	rotation_x -= angle_z;

	float maxAngle = 1.56f; // Just a little bit below PI / 2

	if (rotation_x > maxAngle) {
		rotation_x = maxAngle;
	}
	else if (rotation_x < -maxAngle) {
		rotation_x = -maxAngle;
	}
	else {
		glm::vec3 cross = glm::cross(m_camData.viewpoint - m_camData.position, m_camData.upVector);
		glm::vec3 axis = glm::normalize(cross);

		RotateViewPoint(angle_z, axis);
	}

	RotateViewPoint(angle_y, glm::vec3(0, 1, 0));
}

void PathBuilderCam::RotateViewPoint(float fAngle, const glm::vec3 vPoint)
{
	glm::vec3 vView = m_camData.viewpoint - m_camData.position;

	glm::mat4 R = glm::rotate(glm::mat4(1), fAngle * 180.0f / (float)M_PI, vPoint);
	glm::vec4 newView = R * glm::vec4(vView, 1);

	m_camData.viewpoint = m_camData.position + glm::vec3(newView);
	m_camData.position += glm::normalize(vView);
}
