#include "SideCamera.h"
#include "Game.h"
#include "ModelViewComponent.h"
#include <cmath>

SideCamera::SideCamera()
{
}

camData SideCamera::GetData(double dt, const camData& currentData)
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

			float strafeDistance = 50.0f;
			float followHeight = 16.0f;

			glm::vec3 desiredPosition = targetPos + (right * strafeDistance) + (up * followHeight) + (forward * strafeDistance);
			glm::vec3 desiredViewpoint = targetPos + (forward * 30.0f);

			glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
			glm::vec3 desiredUp = glm::normalize(glm::mix(worldUp, up, 0.3f));

			if (glm::length(currentData.position) < 0.1f)
			{
				newData.position = desiredPosition;
				newData.viewpoint = desiredViewpoint;
				newData.upVector = desiredUp;
			}
			else
			{
				float dtSeconds = (float)(dt / 1000.0);
				float blend = 1.0f - std::exp(-4.0f * dtSeconds);

				newData.position = glm::mix(currentData.position, desiredPosition, blend);
				newData.viewpoint = glm::mix(currentData.viewpoint, desiredViewpoint, blend);
				newData.upVector = glm::normalize(glm::mix(currentData.upVector, desiredUp, blend));
			}
		}
	}

	return newData;
}