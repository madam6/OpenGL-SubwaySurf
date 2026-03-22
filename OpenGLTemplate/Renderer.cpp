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

        shader->SetUniform("matrices.modelViewMatrix", frameData.viewMatrix * renderable.modelMatrix);

        shader->SetUniform("material.Ma", renderable.Ma);
        shader->SetUniform("material.Md", renderable.Md);
        shader->SetUniform("material.Ms", renderable.Ms);

        renderable.mesh->Render();
	}
    m_RenderQueue.clear();
}