#include "ColliderComponent.h"
#include "ComponentRegistry.h"
#include "Entity.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("ColliderComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<ColliderComponent>();
                    c.Apply(props);
                });
            return true;
        }();
}

void ColliderComponent::Init()
{
    if (auto owner = GetOwner()) 
    {
        m_ModelViewRef = owner->FindComponent<ModelViewComponent>();
    }
}

void ColliderComponent::Apply(const PropertyMap& props)
{
    auto itExtents = props.find("extents");
    if (itExtents != props.end()) m_Extents = ParseVec3(itExtents->second);

    auto itOffset = props.find("offset");
    if (itOffset != props.end()) m_Offset = ParseVec3(itOffset->second);
}

glm::vec3 ColliderComponent::GetMinBounds() const
{
    if (!m_ModelViewRef) return glm::vec3(0.0f);

    glm::vec3 center = m_ModelViewRef->GetPosition() + m_Offset;
    glm::vec3 scaledExtents = m_Extents * m_ModelViewRef->GetScale();
    return center - scaledExtents;
}

glm::vec3 ColliderComponent::GetMaxBounds() const
{
    if (!m_ModelViewRef) return glm::vec3(0.0f);

    glm::vec3 center = m_ModelViewRef->GetPosition() + m_Offset;
    glm::vec3 scaledExtents = m_Extents * m_ModelViewRef->GetScale();
    return center + scaledExtents;
}