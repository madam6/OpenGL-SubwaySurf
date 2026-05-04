#include "FirstPersonCam.h"
#include "Game.h"
#include "ModelViewComponent.h"
#include <cmath>

FirstPersonCam::FirstPersonCam()
{
}

camData FirstPersonCam::GetData(double dt, const camData& currentData)
{
	camData newData = currentData;

	if (m_TargetEntity.expired())
	{
		m_TargetEntity = Game::GetInstance().FetchEntityByName("MC");
	}

	if (auto target = m_TargetEntity.lock())
	{
		auto mvComp = target->FindComponent<ModelViewComponent>();
		if (mvComp)
		{
			glm::vec3 targetPos = mvComp->GetPosition();
			glm::mat4 targetOrientation = mvComp->GetOrientation();

			glm::vec3 right(targetOrientation[0]);
			glm::vec3 up(targetOrientation[1]);
			glm::vec3 forward(targetOrientation[2]);

			float followDistance = 18.0f;
			float followHeight = 12.0f;
			glm::vec3 desiredPosition = targetPos + (forward * followDistance) + (up * followHeight);
			glm::vec3 desiredViewpoint = targetPos + (forward * 90.0f);

			glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
			glm::vec3 desiredUp = glm::normalize(glm::mix(worldUp, up, 0.5f));

			float dtSeconds = (float)(dt / 1000.0);

			float blend = 1.0f - std::exp(-8.0f * dtSeconds);

			newData.position = glm::mix(currentData.position, desiredPosition, blend);
			newData.viewpoint = glm::mix(currentData.viewpoint, desiredViewpoint, blend);
			newData.upVector = glm::normalize(glm::mix(currentData.upVector, desiredUp, blend));
		}
	}

	return newData;
}