#include "Renderer.h"
#include "Shaders.h"
#include "OpenAssetImportMesh.h"

void Renderer::Render(const FrameData& frameData)
{
    for (auto& renderable : m_RenderQueue)
    {
        auto* shader = renderable.shader;
        if (shader)
        {
            shader->UseProgram();

            GLuint currentProgram = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&currentProgram);

            DEBUG_MSG(
                "[RENDER] isInstanced=%d | shader ptr=%p | programID=%u | currentlyBound=%u",
                renderable.isInstanced ? 1 : 0,
                shader,
                shader->GetProgramID(),
                currentProgram
            );

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

            if (renderable.isInstanced)
            {
                shader->SetUniform("matrices.projMatrix", frameData.projMatrix);
                shader->SetUniform("matrices.viewMatrix", frameData.viewMatrix);

                renderable.mesh->RenderInstanced(renderable.instanceMatrices);
            }
            else
            {
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
    }
    m_RenderQueue.clear();
}