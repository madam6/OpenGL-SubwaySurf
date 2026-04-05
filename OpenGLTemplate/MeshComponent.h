#pragma once
#include "Component.h"
#include "OpenAssetImportMesh.h"
class ModelViewComponent;
class ShaderComponent;
class MaterialComponent;
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
    std::unique_ptr<COpenAssetImportMesh> m_Mesh;
    ModelViewComponent* m_ModelViewComponentRef = nullptr;
    ShaderComponent* m_ShaderComponentRef = nullptr;
    MaterialComponent* m_MaterialComponent = nullptr;
    //properties
    std::string m_MeshPath;
    bool isFBX;
};