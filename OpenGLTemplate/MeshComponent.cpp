#include "MeshComponent.h"
#include "ModelViewComponent.h"
#include "Entity.h"
#include "ShaderComponent.h"
#include "Game.h"
#include "MaterialComponent.h"
#include "ComponentRegistry.h"
#include "Renderable.h"

namespace
{
	bool registered = []()
		{
			ComponentRegistry::Instance().Register("MeshComponent",
				[](Entity& e, const PropertyMap& props)
				{
					auto& c = e.AddComponent<MeshComponent>();
					c.Apply(props);
				});

			return true;
		}();
}


MeshComponent::MeshComponent()
{
	m_Mesh = std::make_shared<COpenAssetImportMesh>();
}

void MeshComponent::Init()
{
	if (auto owner = GetOwner())
	{
		m_ModelViewComponentRef = owner->FindComponent<ModelViewComponent>();
		m_ShaderComponentRef = owner->FindComponent<ShaderComponent>();
		m_MaterialComponent = owner->FindComponent<MaterialComponent>();
	}

}

void MeshComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
	RenderData data;
	data.mesh = m_Mesh;
	data.useTexture = true;
	data.modelMatrix = glm::mat4(1.0f);

	if (m_ModelViewComponentRef)
	{
		data.modelMatrix = glm::translate(data.modelMatrix, m_ModelViewComponentRef->GetPosition());
		data.modelMatrix *= m_ModelViewComponentRef->GetOrientation();
		data.modelMatrix = glm::scale(data.modelMatrix, m_ModelViewComponentRef->GetScale());
	}

	if (m_ShaderComponentRef)
	{
		data.shader = Game::GetInstance().GetShader(m_ShaderComponentRef->GetShaderName());
	}
	else
	{
		data.shader = Game::GetInstance().GetShader(MAIN_SHADER);
	}

	if (m_MaterialComponent)
	{
		data.Ma = m_MaterialComponent->GetMa();
		data.Md = m_MaterialComponent->GetMd();
		data.Ms = m_MaterialComponent->GetMs();
		data.shininess = m_MaterialComponent->GetShiny();
	}
	
	renderQueue.push_back(std::move(data));
}

void MeshComponent::Apply(const PropertyMap& props)
{
	auto itFbx = props.find("isfbx");
	auto itFilePath = props.find("mesh_file_path");
	auto itSpeed = props.find("anim_speed");
	if (itFbx != props.end() && itFilePath != props.end() && itSpeed != props.end())
	{
		if (itFbx->second == "true")
		{
			m_Mesh->LoadFBX(itFilePath->second);
			isFBX = true;
		}
		else
		{
			m_Mesh->Load(itFilePath->second);
			isFBX = false;
		}
		m_Mesh->SetAnimationSpeed(std::stof(itSpeed->second));

	}
	else
	{
		LOG_ERROR("Incorrect properties on mesh component");
		assert(0);
	}
}

void MeshComponent::Update(float dt)
{
	if (isFBX)
	{
		m_Mesh->UpdateAnimation(dt);
	}
}
