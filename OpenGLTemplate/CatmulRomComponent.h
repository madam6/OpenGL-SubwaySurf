#pragma once
#include "Component.h"
#include "CatmullRom.h"

class PlayerTrackComponent;
class MaterialComponent;
class CatmullRomComponent : public Component
{
public:

    void Init() override;

    void AddRenderData(std::vector<RenderData>& renderQueue) override;

    void Apply(const PropertyMap& props);
    void Update(float dt) override;
private:
    std::shared_ptr<CCatmullRom> m_track;
    std::shared_ptr<MaterialComponent> m_MaterialComponent = nullptr;
    float m_distance;
    float m_speed{ 1.f };
    float m_distanceFactor;
    float m_heightFactor;
    glm::vec3 m_position;
    std::string m_file;

    friend class PlayerTrackMovementComponent;
    friend class CurrencyManagerComponent;
};