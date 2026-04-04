#pragma once
#include "Component.h"
class COpenAssetImportMesh;
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
private:
    std::unique_ptr<COpenAssetImportMesh> m_Mesh;
    std::shared_ptr<ModelViewComponent> m_ModelViewComponentRef;
    std::shared_ptr<ShaderComponent> m_ShaderComponentRef;
    std::shared_ptr<MaterialComponent> m_MaterialComponent;
    //properties
    std::string m_MeshPath;
    bool isFBX;
};