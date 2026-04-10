#pragma once
#include "Component.h"
#include "Crystal.h"
#include "ModelViewComponent.h"
#include "ShaderComponent.h"

class CrystalMeshComponent : public Component
{
public:
    void Init() override;
    void AddRenderData(std::vector<RenderData>& renderQueue) override;
    void Apply(const PropertyMap& props) {}

private:
    static std::shared_ptr<CCrystal> s_SharedCrystal; // Shared so we only load the CSV once!
    std::shared_ptr<ModelViewComponent> m_ModelViewRef;
    std::shared_ptr<ShaderComponent> m_ShaderRef;
};
