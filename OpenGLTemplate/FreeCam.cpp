#include "FreeCam.h"
#include "gamewindow.h"
#include "Common.h"

FreeCam::FreeCam()
{
	m_camData.position = glm::vec3();
	m_camData.upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_camData.viewpoint = glm::vec3(0.0f, 0.0f, -1.0f);
}

camData FreeCam::GetData(double dt, const camData& currentData)
{
	m_camData = currentData;
	SetViewByMouse();
	return m_camData;
}

void FreeCam::SetViewByMouse()
{
	int middle_x = GameWindow::SCREEN_WIDTH >> 1;
	int middle_y = GameWindow::SCREEN_HEIGHT >> 1;

	POINT mouse;
	GetCursorPos(&mouse);

	if (mouse.x == middle_x && mouse.y == middle_y)
	{
		return;
	}

	SetCursorPos(middle_x, middle_y);

	float delta_x = (float)(middle_x - mouse.x);
	float delta_y = (float)(middle_y - mouse.y);

	float sensitivity = 0.0015f;

	glm::vec3 viewDir = glm::normalize(m_camData.viewpoint - m_camData.position);

	float pitch = std::asin(viewDir.y);
	float yaw = std::atan2(viewDir.z, viewDir.x);

	yaw -= delta_x * sensitivity;
	pitch += delta_y * sensitivity;

	float maxPitch = glm::radians(89.0f);
	if (pitch > maxPitch) pitch = maxPitch;
	if (pitch < -maxPitch) pitch = -maxPitch;

	glm::vec3 newView;
	newView.x = std::cos(yaw) * std::cos(pitch);
	newView.y = std::sin(pitch);
	newView.z = std::sin(yaw) * std::cos(pitch);

	m_camData.viewpoint = m_camData.position + glm::normalize(newView);

	m_camData.upVector = glm::vec3(0.0f, 1.0f, 0.0f);
}