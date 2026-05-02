#include "PlayerTrackMovementComponent.h"
#include "ComponentRegistry.h"
#include "ModelViewComponent.h"
#include "CatmulRomComponent.h"
#include "Entity.h"
#include "Game.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("PlayerMovementComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<PlayerTrackMovementComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}

void PlayerTrackMovementComponent::Init()
{
    if (auto owner = GetOwner())
    {
        m_ModelViewComponentRef = owner->FindComponent<ModelViewComponent>();
        assert(m_ModelViewComponentRef);
    }

    if (auto track = Game::GetInstance().FetchEntityByName("CatmullRomTrack"))
    {
        m_CatmullRomComponentRef = track->FindComponent<CatmullRomComponent>();
        assert(m_CatmullRomComponentRef);
    }

    EventSystem::Instance().Subscribe("KeyDown", this);
}

void PlayerTrackMovementComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
}

void PlayerTrackMovementComponent::Update(float dt)
{
    float dtSeconds = dt / 1000.0f;
    
    if (m_IsSpeedBoostActive)
    {
        m_SpeedBoostTimer += dt;
        if (m_SpeedBoostTimer / m_SpeedBoostTimeOut >= 1.0f)
        {
            m_IsSpeedBoostActive = false;
            m_Speed -= m_SpeedBoost;
            m_SpeedBoostTimer = 0.f;
        }
    }

    if (m_IsRecovering)
    {
        m_RecoveryTimer += dtSeconds;
        float t = m_RecoveryTimer / m_RecoveryDuration;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_IsRecovering = false;
            m_CurrentDistance = m_RecordedDistance;
            m_CurrentLaneOffset = m_RecordedLaneOffset;
            m_TargetLane = m_RecordedLane;
        }
        else
        {
            m_CurrentDistance = glm::mix(m_StartRecoveryDistance, m_RecordedDistance, t);
            m_CurrentLaneOffset = glm::mix(m_StartRecoveryLaneOffset, m_RecordedLaneOffset, t);
        }
    }
    else
    {
        m_CurrentDistance += dt * m_Speed;

        m_RecordTimer += dtSeconds;
        if (m_RecordTimer >= 3.0f)
        {
            m_RecordTimer = 0.0f;
            m_RecordedDistance = m_CurrentDistance;
            m_RecordedLane = m_TargetLane;
            m_RecordedLaneOffset = m_CurrentLaneOffset;
        }

        float targetOffset = m_TargetLane * m_LaneWidth;
        m_CurrentLaneOffset = targetOffset + (m_CurrentLaneOffset - targetOffset) * std::exp(-15.0f * dtSeconds);
    }

    glm::vec3 p, up;
    m_CatmullRomComponentRef->m_track->Sample(m_CurrentDistance, p, up);

    glm::vec3 pNext;
    m_CatmullRomComponentRef->m_track->Sample(m_CurrentDistance + 1.f, pNext);

    glm::vec3 forward = glm::normalize(pNext - p);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 realUp = glm::normalize(glm::cross(right, forward));

    glm::vec3 finalPos = p + (right * m_CurrentLaneOffset) + (realUp * m_HeightFactor);
    glm::mat4 orientation = glm::mat4(
        glm::vec4(right, 0.0f),
        glm::vec4(realUp, 0.0f),
        glm::vec4(forward, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    m_ModelViewComponentRef->SetPosition(finalPos);
    m_ModelViewComponentRef->SetOrientation(orientation);
}

void PlayerTrackMovementComponent::OnEvent(const std::string& eventName, const EventData& data)
{
    if (eventName == "KeyDown")
    {
        int key = std::any_cast<int>(data.payload);

        if (key == 'A' || key == VK_LEFT) 
        {
            m_TargetLane = std::max(-1, m_TargetLane - 1);
        }

        else if (key == 'D' || key == VK_RIGHT) 
        {
            m_TargetLane = std::min(1, m_TargetLane + 1);
        }
    }
}

void PlayerTrackMovementComponent::Apply(const PropertyMap& props)
{
    auto speedIt = props.find("speed");
    if (speedIt != props.end()) m_Speed = std::stof(speedIt->second);

    auto heightIt = props.find("height_factor");
    if (heightIt != props.end()) m_HeightFactor = std::stof(heightIt->second);
}

void PlayerTrackMovementComponent::StartRecovery()
{
    if (m_IsRecovering) return;
    m_IsRecovering = true;
    m_RecoveryTimer = 0.0f;

    m_StartRecoveryDistance = m_CurrentDistance;
    m_StartRecoveryLaneOffset = m_CurrentLaneOffset;
}

void PlayerTrackMovementComponent::ApplySpeedBoost(float milliSeconds)
{
    if (!m_IsSpeedBoostActive)
    {
        m_IsSpeedBoostActive = true;
        m_SpeedBoostTimeOut = milliSeconds;
        m_Speed += m_SpeedBoost;
    }
}
