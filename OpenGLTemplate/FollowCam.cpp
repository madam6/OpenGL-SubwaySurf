#include "FollowCam.h"
#include "Game.h"
#include "ModelViewComponent.h"

FollowCam::FollowCam()
{
}

camData FollowCam::GetData(double dt, const camData& currentData)
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

			float followDistance = 30.0f;
			float followHeight = 15.0f;

			newData.position = targetPos - (forward * followDistance) + (up * followHeight);
			newData.viewpoint = targetPos + (forward * 10.0f);
			newData.upVector = up;
		}
	}

	return newData;
}