#pragma once
#include "Component.h"
#include "CatmullRom.h"

class CatmullRomComponent : public Component
{
public:

    void Init() override;

    void AddRenderData(std::vector<RenderData>& renderQueue) override;

    void Apply(const PropertyMap& props);
    void Update(float dt) override;
private:
    CCatmullRom m_track;
    float m_distance;
    float m_speed{ 1.f };
    float m_distanceFactor;
    float m_heightFactor;
    glm::vec3 m_position;
    std::string m_file;
};