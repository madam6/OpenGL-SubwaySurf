#include "TopDownCam.h"
#include "Game.h"
#include "ModelViewComponent.h"
#include <cmath>

TopDownCam::TopDownCam() {}

camData TopDownCam::GetData(double dt, const camData& currentData)
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

			glm::vec3 gorillaForward(targetOrientation[2]);

			float viewHeight = 200.0f;
			glm::vec3 desiredPosition = targetPos + glm::vec3(0.0f, viewHeight, 0.0f);
			glm::vec3 desiredViewpoint = targetPos;

			glm::vec3 desiredUp = glm::normalize(gorillaForward);

			float dtSeconds = (float)(dt / 1000.0);

			float blend = 1.0f - std::exp(-5.0f * dtSeconds);

			newData.position = glm::mix(currentData.position, desiredPosition, blend);
			newData.viewpoint = glm::mix(currentData.viewpoint, desiredViewpoint, blend);
			newData.upVector = glm::normalize(glm::mix(currentData.upVector, desiredUp, blend));
		}
	}

	return newData;
}
