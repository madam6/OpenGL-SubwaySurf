#pragma once
#include "Component.h"
#include "OpenAssetImportMesh.h"
class ModelViewComponent;
class ShaderComponent;
class MaterialComponent;

// Component that allows to load meshes into the world inclduing .fbx and .obj formates
// Just specify mesh path and flag of its format
class MeshComponent : public Component
{
public:
    MeshComponent();
    void Init() override;

    void AddRenderData(std::vector<RenderData>& renderQueue) override;

    void Apply(const PropertyMap& props);
    void Update(float dt) override;
    void SetAnimationSpeed(float speed) { if (m_Mesh) m_Mesh->SetAnimationSpeed(speed); }
private:
    std::shared_ptr<COpenAssetImportMesh> m_Mesh;
    std::shared_ptr<ModelViewComponent> m_ModelViewComponentRef = nullptr;
    std::shared_ptr<ShaderComponent> m_ShaderComponentRef = nullptr;
    std::shared_ptr<MaterialComponent> m_MaterialComponent = nullptr;
    //properties
    std::string m_MeshPath;
    bool isFBX;
};