#include "CrystalMeshComponent.h"
#include "Entity.h"
#include "Game.h"
#include "ComponentRegistry.h"

std::shared_ptr<CCrystal> CrystalMeshComponent::s_SharedCrystal = nullptr;

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("CrystalMeshComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<CrystalMeshComponent>();
                    c.Apply(props);
                });
            return true;
        }();
}

void CrystalMeshComponent::Init() 
{
    if (!s_SharedCrystal) 
    {
        s_SharedCrystal = std::make_shared<CCrystal>();
        s_SharedCrystal->Create("resources\\textures\\", "crystalTexture.jpg");
    }

    if (auto owner = GetOwner()) 
    {
        m_ModelViewRef = owner->FindComponent<ModelViewComponent>();
        m_ShaderRef = owner->FindComponent<ShaderComponent>();
    }
}

void CrystalMeshComponent::AddRenderData(std::vector<RenderData>& renderQueue) 
{
    RenderData data;
    data.mesh = s_SharedCrystal;
    data.modelMatrix = glm::mat4(1.0f);
    data.useTexture = true;

    if (m_ModelViewRef) 
    {
        data.modelMatrix = glm::translate(data.modelMatrix, m_ModelViewRef->GetPosition());
        data.modelMatrix *= m_ModelViewRef->GetOrientation();
        data.modelMatrix = glm::scale(data.modelMatrix, m_ModelViewRef->GetScale());
    }

    if (m_ShaderRef)
    {
        data.shader = Game::GetInstance().GetShader(m_ShaderRef->GetShaderName());
    }
    else
    {
        data.shader = Game::GetInstance().GetShader("CrystalShader");
    }

    renderQueue.push_back(std::move(data));
}