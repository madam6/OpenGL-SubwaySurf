#include "camera.h"
#include "gamewindow.h"
#include "CameraMode.h"
#include "FreeCam.h"
#include "PathBuilderCam.h"

// Constructor for camera -- initialise with some default values
CCamera::CCamera()
{
	m_CamData.position = glm::vec3(0.0f, 10.0f, 100.0f);
	m_CamData.viewpoint = glm::vec3(0.0f, 0.0f, 0.0f);
	m_CamData.upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_CameraModes.reserve(5);
	m_CameraModes.push_back(std::make_unique<FreeCam>());
	m_CameraModes.push_back(std::make_unique<PathBuilderCam>());
	m_CameraModeIndex = 0;
	m_speed = 0.025f;
}
CCamera::~CCamera()
{}
 
// Set the camera at a specific position, looking at the view point, with a given up vector
void CCamera::Set(const camData& camData)
{
	m_CamData = camData;
}

// Strafe the camera (side to side motion)
void CCamera::Strafe(double direction)
{
	float speed = (float) (m_speed * direction);

	m_CamData.position.x = m_CamData.position.x + m_strafeVector.x * speed;
	m_CamData.position.z = m_CamData.position.z + m_strafeVector.z * speed;

	m_CamData.viewpoint.x = m_CamData.viewpoint.x + m_strafeVector.x * speed;
	m_CamData.viewpoint.z = m_CamData.viewpoint.z + m_strafeVector.z * speed;
}

// Advance the camera (forward / backward motion)
void CCamera::Advance(double direction)
{
	float speed = (float) (m_speed * direction);

	glm::vec3 view = glm::normalize(m_CamData.viewpoint - m_CamData.position);
	m_CamData.position = m_CamData.position + view * speed;
	m_CamData.viewpoint = m_CamData.viewpoint + view * speed;

}

// Update the camera to respond to mouse motion for rotations and keyboard for translation
void CCamera::Update(double dt)
{
	glm::vec3 vector = glm::cross(m_CamData.viewpoint - m_CamData.position, m_CamData.upVector);
	m_strafeVector = glm::normalize(vector);

	m_CamData = m_CameraModes[m_CameraModeIndex]->GetData(dt, m_CamData);

	TranslateByKeyboard(dt);
}

// Update the camera to respond to key presses for translation
void CCamera::TranslateByKeyboard(double dt)
{
	if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
		Advance(1.0*dt);
	}

	if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {
		Advance(-1.0*dt);
	}

	if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {
		Strafe(-1.0*dt);
	}

	if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
		Strafe(1.0*dt);
	}

	if (GetKeyState('2') & 0x80)
	{
		IncrementCameraIndexSafe();
	}

	if (GetKeyState('1') & 0x80)
	{
		DecrementCameraIndexSafe();
	}
}
// Return the camera position
glm::vec3 CCamera::GetPosition() const
{
	return m_CamData.position;
}

// Return the camera view point
glm::vec3 CCamera::GetView() const
{
	return m_CamData.viewpoint;
}

// Return the camera up vector
glm::vec3 CCamera::GetUpVector() const
{
	return m_CamData.upVector;
}

// Return the camera strafe vector
glm::vec3 CCamera::GetStrafeVector() const
{
	return m_strafeVector;
}

// Return the camera perspective projection matrix
glm::mat4* CCamera::GetPerspectiveProjectionMatrix()
{
	return &m_perspectiveProjectionMatrix;
}

// Return the camera orthographic projection matrix
glm::mat4* CCamera::GetOrthographicProjectionMatrix()
{
	return &m_orthographicProjectionMatrix;
}

// Set the camera perspective projection matrix to produce a view frustum with a specific field of view, aspect ratio, 
// and near / far clipping planes
void CCamera::SetPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	m_perspectiveProjectionMatrix = glm::perspective(fov, aspectRatio, nearClippingPlane, farClippingPlane);
}

// The the camera orthographic projection matrix to match the width and height passed in
void CCamera::SetOrthographicProjectionMatrix(int width, int height)
{
	m_orthographicProjectionMatrix = glm::ortho(0.0f, float(width), 0.0f, float(height));
}

// Get the camera view matrix
glm::mat4 CCamera::GetViewMatrix()
{
	return glm::lookAt(m_CamData.position, m_CamData.viewpoint, m_CamData.upVector);
}

// The normal matrix is used to transform normals to eye coordinates -- part of lighting calculations
glm::mat3 CCamera::ComputeNormalMatrix(const glm::mat4 &modelViewMatrix)
{
	return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
}

void CCamera::IncrementCameraIndexSafe()
{
	m_CameraModeIndex = (m_CameraModeIndex + 1) % m_CameraModes.size();
}

void CCamera::DecrementCameraIndexSafe()
{
	m_CameraModeIndex = (m_CameraModeIndex - 1) % m_CameraModes.size();
}

