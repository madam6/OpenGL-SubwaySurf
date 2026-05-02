#include "Renderer.h"
#include "Shaders.h"
#include "OpenAssetImportMesh.h"

void Renderer::Render(const FrameData& frameData)
{
    // A map to group instances by their underlying mesh & shader
    std::map<std::pair<Renderable*, CShaderProgram*>, RenderData> instancedBatches;

    for (auto& renderable : m_RenderQueue)
    {
        if (renderable.isInstanced && renderable.mesh)
        {
            std::pair<Renderable*, CShaderProgram*> key = std::make_pair(renderable.mesh.get(), renderable.shader);

            if (instancedBatches.find(key) == instancedBatches.end())
            {
                instancedBatches[key] = renderable;
            }
            else
            {
                for (const auto& mat : renderable.instanceMatrices)
                {
                    instancedBatches[key].instanceMatrices.push_back(mat);
                }
            }
            continue;
        }

        auto* shader = renderable.shader;
        if (shader)
        {
            shader->UseProgram();
            shader->SetUniform("matrices.projMatrix", frameData.projMatrix);
            shader->SetUniform("matrices.viewMatrix", frameData.viewMatrix);
            shader->SetUniform("numLights", (int)frameData.lights.size());

            for (int i = 0; i < frameData.lights.size(); i++)
            {
                std::string base = "lights[" + std::to_string(i) + "]";
                shader->SetUniform(base + ".position", frameData.viewMatrix * frameData.lights[i].position);
                shader->SetUniform(base + ".La", frameData.lights[i].La);
                shader->SetUniform(base + ".Ld", frameData.lights[i].Ld);
                shader->SetUniform(base + ".Ls", frameData.lights[i].Ls);
            }

            shader->SetUniform("bUseTexture", renderable.useTexture ? 1 : 0);
            shader->SetUniform("sampler0", 0);
            shader->SetUniform("uTime", frameData.time);
            shader->SetUniform("uIsRecovering", renderable.isRecovering ? 1 : 0);

            shader->SetUniform("material1.Ma", renderable.Ma);
            shader->SetUniform("material1.Md", renderable.Md);
            shader->SetUniform("material1.Ms", renderable.Ms);
            shader->SetUniform("material1.shininess", renderable.shininess);

            if (renderable.isOutline)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glFrontFace(GL_CW);
            }
            else 
            {
                glDisable(GL_CULL_FACE);
                glFrontFace(GL_CCW);
            }

            glm::mat4 modelView = frameData.viewMatrix * renderable.modelMatrix;
            shader->SetUniform("matrices.modelViewMatrix", modelView);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
            shader->SetUniform("matrices.normalMatrix", normalMatrix);

            if (auto mesh = std::dynamic_pointer_cast<COpenAssetImportMesh>(renderable.mesh)) 
            {
                int numBones = mesh->GetNumBones();
                if (numBones > 0)
                {
                    const std::vector<BoneInfo>& boneInfo = mesh->GetBoneInfo();
                    std::vector<glm::mat4> boneTransforms(numBones);
                    for (int i = 0; i < numBones; ++i) {
                        boneTransforms[i] = boneInfo[i].finalTransform;
                    }
                    shader->SetUniform("u_BoneTransforms", boneTransforms.data(), numBones);
                }
            }

            renderable.mesh->Render();
        }
    }

    for (auto& pair : instancedBatches)
    {
        auto& renderable = pair.second;
        auto* shader = renderable.shader;
        if (!shader) continue;

        shader->UseProgram();
        shader->SetUniform("matrices.projMatrix", frameData.projMatrix);
        shader->SetUniform("matrices.viewMatrix", frameData.viewMatrix);
        shader->SetUniform("numLights", (int)frameData.lights.size());

        for (int i = 0; i < frameData.lights.size(); i++)
        {
            std::string base = "lights[" + std::to_string(i) + "]";
            shader->SetUniform(base + ".position", frameData.viewMatrix * frameData.lights[i].position);
            shader->SetUniform(base + ".La", frameData.lights[i].La);
            shader->SetUniform(base + ".Ld", frameData.lights[i].Ld);
            shader->SetUniform(base + ".Ls", frameData.lights[i].Ls);
        }

        shader->SetUniform("bUseTexture", renderable.useTexture ? 1 : 0);
        shader->SetUniform("sampler0", 0);
        shader->SetUniform("uTime", frameData.time);

        shader->SetUniform("material1.Ma", renderable.Ma);
        shader->SetUniform("material1.Md", renderable.Md);
        shader->SetUniform("material1.Ms", renderable.Ms);
        shader->SetUniform("material1.shininess", renderable.shininess);

        if (renderable.isOutline)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
        }

        renderable.mesh->RenderInstanced(renderable.instanceMatrices);
    }

    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    m_RenderQueue.clear();
}