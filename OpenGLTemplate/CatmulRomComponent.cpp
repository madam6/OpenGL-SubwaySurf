#include "CatmulRomComponent.h"
#include "ComponentRegistry.h"
#include "Entity.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("CatmullRomComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<CatmullRomComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}

void CatmullRomComponent::Init()
{
    m_track = std::make_shared<CCatmullRom>();
    m_track->SetTrackFile(m_file);
    m_track->CreateCentreline();

    m_track->CreateOffsetCurves();

    m_track->CreateTrack("resources\\textures\\", "road.jpg");
}

void CatmullRomComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
    RenderData data;
    data.mesh = m_track;

    renderQueue.push_back(std::move(data));
}

void CatmullRomComponent::Apply(const PropertyMap& props)
{
    auto it = props.find("file");
    if (it != props.end())
    {
        m_file = it->second;
    }
}

void CatmullRomComponent::Update(float dt)
{
    m_distance += dt * m_speed;

    glm::vec3 p, pNext;
    if (!m_track->Sample(m_distance, p)) return;
    if (!m_track->Sample(m_distance + 1.f, pNext)) return;

    glm::vec3 tangentVectorT = glm::normalize(pNext - p);
    glm::vec3 N = glm::normalize(glm::cross(tangentVectorT, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 B = glm::normalize(glm::cross(N, tangentVectorT));

    m_position = p + (m_distanceFactor * N) + (m_heightFactor * B);
}
