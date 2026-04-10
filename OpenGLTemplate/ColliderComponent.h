#pragma once
#include "Component.h"
#include "Common.h"
#include "ModelViewComponent.h"

class ColliderComponent : public Component
{
public:
    void Init() override;
    void Apply(const PropertyMap& props);

    glm::vec3 GetMinBounds() const;
    glm::vec3 GetMaxBounds() const;
    void SetActive(bool active) { m_IsActive = active; }
    bool IsActive() const { return m_IsActive; }
private:
    std::shared_ptr<ModelViewComponent> m_ModelViewRef;
    bool m_IsActive = true;
    // propertiess
    glm::vec3 m_Extents{ 1.0f, 1.0f, 1.0f };
    glm::vec3 m_Offset{ 0.0f, 0.0f, 0.0f };
};