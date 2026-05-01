#include "TreeMeshComponent.h"
#include "Entity.h"
#include "Game.h"
#include "ComponentRegistry.h"

std::shared_ptr<COpenAssetImportMesh> TreeMeshComponent::s_SharedTree = nullptr;

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("TreeMeshComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<TreeMeshComponent>();
                    c.Apply(props);
                });
            return true;
        }();
}

void TreeMeshComponent::Init()
{
    if (!s_SharedTree)
    {
        s_SharedTree = std::make_shared<COpenAssetImportMesh>();
        s_SharedTree->LoadFBX("resources\\models\\Palm\\Palm_Tree_1.fbx");
    }

    if (auto owner = GetOwner())
    {
        m_ModelViewRef = owner->FindComponent<ModelViewComponent>();
        m_ShaderRef = owner->FindComponent<ShaderComponent>();
        m_MaterialRef = owner->FindComponent<MaterialComponent>();
    }
}

void TreeMeshComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
    if (!m_ModelViewRef) return;

    RenderData data;
    data.mesh = s_SharedTree;
    data.useTexture = true;
    data.isInstanced = true;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_ModelViewRef->GetPosition());
    model *= m_ModelViewRef->GetOrientation();
    model = glm::scale(model, m_ModelViewRef->GetScale());

    data.instanceMatrices.push_back(model);

    if (m_ShaderRef) 
    {
        data.shader = Game::GetInstance().GetShader(m_ShaderRef->GetShaderName());
    }
    else 
    {
        data.shader = Game::GetInstance().GetShader("MainShader");
    }

    if (m_MaterialRef) 
    {
        data.Ma = m_MaterialRef->GetMa();
        data.Md = m_MaterialRef->GetMd();
        data.Ms = m_MaterialRef->GetMs();
        data.shininess = m_MaterialRef->GetShiny();
    }

    renderQueue.push_back(std::move(data));
}