#include "MeshComponent.h"
#include "OpenAssetImportMesh.h"
#include "ModelViewComponent.h"
#include "Entity.h"
#include "ShaderComponent.h"
#include "Game.h"
#include "MaterialComponent.h"

MeshComponent::MeshComponent()
{
	m_Mesh = std::make_unique<COpenAssetImportMesh>();
}

void MeshComponent::Init()
{
	if (GetOwner() != nullptr)
	{
		m_ModelViewComponentRef = std::make_shared<ModelViewComponent>(GetOwner()->FindComponent<ModelViewComponent>());
		m_ShaderComponentRef = std::make_shared<ShaderComponent>(GetOwner()->FindComponent<ShaderComponent>());
		m_MaterialComponent = std::make_shared<MaterialComponent>(GetOwner()->FindComponent<MaterialComponent>());
	}

}

void MeshComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
	RenderData data;
	data.mesh = m_Mesh.get();

	data.modelMatrix = glm::mat4(1.0f);

	if (m_ModelViewComponentRef)
	{
		data.modelMatrix = glm::translate(data.modelMatrix, m_ModelViewComponentRef->GetPosition());
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
	
	renderQueue.push_back(data);
}

void MeshComponent::Apply(const PropertyMap& props)
{
	auto itFbx = props.find("isfbx");
	auto itFilePath = props.find("mesh_file_path");
	if (itFbx != props.end() && itFilePath != props.end())
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
