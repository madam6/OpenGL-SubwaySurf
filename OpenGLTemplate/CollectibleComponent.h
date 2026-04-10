#pragma once
#include "Component.h"
#include "ModelViewComponent.h"

class CollectibleComponent : public Component
{
public:
    void Init() override;
    void Update(float dt) override;
    void Apply(const PropertyMap& props) {}

    void SpawnAt(glm::vec3 position, glm::mat4 orientation);
    void Collect();
    bool IsCollected() const { return m_IsCollected; }

private:
    std::shared_ptr<ModelViewComponent> m_ModelViewRef;
    glm::vec3 m_BasePosition{ 0.0f };
    glm::mat4 m_BaseOrientation{ 1.0f };
    float m_FloatTimer = 0.0f;
    bool m_IsCollected = false;
};