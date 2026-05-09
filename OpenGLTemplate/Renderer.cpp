#include "Renderer.h"
#include "Shaders.h"
#include "OpenAssetImportMesh.h"
#include "FrameBufferObject.h"

void Renderer::Render(const FrameData& frameData)
{
    // Groups instanced objects by mesh + shader so they can be drawn
    // with a single instanced draw call
    std::map<std::pair<Renderable*, CShaderProgram*>, RenderData> instancedBatches;

    for (auto& renderable : m_RenderQueue)
    {
        // Collect instanced renderables into batches instead of rendering theb one by one
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
            shader->SetUniform("bIsDepthPass", frameData.isDepthPass ? 1 : 0);
            shader->SetUniform("lightSpaceMatrix", frameData.lightSpaceMatrix);

            if (!frameData.isDepthPass && m_DepthBuffer)
            {
                int shadowMapTextureUnit = 11;
                // Bind shadow map generated during the depth pass
                m_DepthBuffer->BindDepth(shadowMapTextureUnit);
                shader->SetUniform("shadowMap", shadowMapTextureUnit);
            }

            for (int i = 0; i < frameData.lights.size(); i++)
            {
                std::string base = "lights[" + std::to_string(i) + "]";
                // Transform light positions into view space for lighting calculations
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
            // Correctly transforms normals under non-uniform scaling
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
            shader->SetUniform("matrices.normalMatrix", normalMatrix);
            shader->SetUniform("modelMatrix", renderable.modelMatrix);
            // Animated meshes upload their final bone transforms each frame
            // The vertex shader uses these matrices for GPU skinning
            if (auto mesh = std::dynamic_pointer_cast<COpenAssetImportMesh>(renderable.mesh)) 
            {
                int numBones = mesh->GetNumBones();
                if (numBones > 0)
                {
                    const std::vector<BoneInfo>& boneInfo = mesh->GetBoneInfo();
                    std::vector<glm::mat4> boneTransforms(numBones);
                    for (int i = 0; i < numBones; ++i) {
                        // finalTransform = animatedPose * inverseBindPose
                        boneTransforms[i] = boneInfo[i].finalTransform;
                    }
                    // Upload skeletal pose matrices to the shader
                    shader->SetUniform("u_BoneTransforms", boneTransforms.data(), numBones);
                }
            }

            renderable.mesh->Render();
        }
    }

    // Render all collected instance batches using instanced rendering
    for (auto& pair : instancedBatches)
    {
        auto& renderable = pair.second;
        auto* shader = renderable.shader;
        if (!shader) continue;

        shader->UseProgram();
        shader->SetUniform("lightSpaceMatrix", frameData.lightSpaceMatrix);
        shader->SetUniform("matrices.projMatrix", frameData.projMatrix);
        shader->SetUniform("matrices.viewMatrix", frameData.viewMatrix);
        shader->SetUniform("numLights", (int)frameData.lights.size());
        shader->SetUniform("bIsDepthPass", frameData.isDepthPass ? 1 : 0);

        if (!frameData.isDepthPass && m_DepthBuffer)
        {
            int shadowMapTextureUnit = 11;
            m_DepthBuffer->BindDepth(shadowMapTextureUnit);
            shader->SetUniform("shadowMap", shadowMapTextureUnit);
        }

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

        // Front face culling on slightly bigger meshes
        // produces an outline/silhouette effect with fragment shader that paints every fragment black
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

    // Restore default render state to avoid leaking state into subsequent render passes
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    m_RenderQueue.clear();
}