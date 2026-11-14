#pragma once
#include "ECS/Component.h"
#include "Rendering/OpenAssetImportMesh.h"
#include "ECS/Components/ModelViewComponent.h"
#include "ECS/Components/ShaderComponent.h"
#include "ECS/Components/MaterialComponent.h"

// Component wrapper of instanced tree meshes
class TreeMeshComponent : public Component
{
public:
    void Init() override;
    void AddRenderData(std::vector<RenderData>& renderQueue) override;
    void Apply(const PropertyMap& props) {}

private:
    static std::shared_ptr<COpenAssetImportMesh> s_SharedTree;
    std::shared_ptr<ModelViewComponent> m_ModelViewRef;
    std::shared_ptr<ShaderComponent> m_ShaderRef;
    std::shared_ptr<MaterialComponent> m_MaterialRef;
};