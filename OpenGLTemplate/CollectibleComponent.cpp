#include "CollectibleComponent.h"
#include "Entity.h"
#include "ComponentRegistry.h"
#include "ColliderComponent.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("CollectibleComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<CollectibleComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}

void CollectibleComponent::Init() 
{
    if (auto owner = GetOwner()) m_ModelViewRef = owner->FindComponent<ModelViewComponent>();
}

void CollectibleComponent::Update(float dt) 
{

}

void CollectibleComponent::SpawnAt(glm::vec3 position, glm::mat4 orientation)
{
    m_IsCollected = false;
    m_BasePosition = position;
    m_BaseOrientation = orientation;
    if (m_ModelViewRef) 
    {
        m_ModelViewRef->SetPosition(position);
        m_ModelViewRef->SetOrientation(orientation);
    }
    auto collider = GetOwner()->FindComponent<ColliderComponent>();
    if (collider) collider->SetActive(true);
}

void CollectibleComponent::Collect() 
{
    m_IsCollected = true;
    if (m_ModelViewRef) m_ModelViewRef->SetPosition(glm::vec3(0, -10000.0f, 0));
    auto collider = GetOwner()->FindComponent<ColliderComponent>();
    if (collider) collider->SetActive(false);
}