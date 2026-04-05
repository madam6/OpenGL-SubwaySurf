#include "Renderer.h"
#include "Shaders.h"
#include "OpenAssetImportMesh.h"

void Renderer::Render(const FrameData& frameData)
{
	for (auto& renderable : m_RenderQueue)
	{
        auto* shader = renderable.shader;
        shader->UseProgram();

        shader->SetUniform("matrices.projMatrix", frameData.projMatrix);
        shader->SetUniform("matrices.viewMatrix", frameData.viewMatrix);

        for(int i = 0; i < frameData.lights.size(); i++)
        {
            std::string base = "lights[" + std::to_string(i) + "]";

            shader->SetUniform(base + ".position", frameData.viewMatrix * frameData.lights[i].position);
            shader->SetUniform(base + ".La", frameData.lights[i].La);
            shader->SetUniform(base + ".Ld", frameData.lights[i].Ld);
            shader->SetUniform(base + ".Ls", frameData.lights[i].Ls);
        }
        glm::mat4 modelView = frameData.viewMatrix * renderable.modelMatrix;
        shader->SetUniform("matrices.modelViewMatrix", modelView);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
        shader->SetUniform("matrices.normalMatrix", normalMatrix);

        shader->SetUniform("material.Ma", renderable.Ma);
        shader->SetUniform("material.Ma", renderable.Ma);
        shader->SetUniform("material.Md", renderable.Md);
        shader->SetUniform("material.Ms", renderable.Ms);

        int numBones = renderable.mesh->GetNumBones();
        if (numBones > 0)
        {
            const std::vector<BoneInfo>& boneInfo = renderable.mesh->GetBoneInfo();

            std::vector<glm::mat4> boneTransforms(numBones);
            for (int i = 0; i < numBones; ++i)
            {
                boneTransforms[i] = boneInfo[i].finalTransform;
            }

            shader->SetUniform("u_BoneTransforms", boneTransforms.data(), numBones);
        }

        renderable.mesh->Render();
	}
    m_RenderQueue.clear();
}