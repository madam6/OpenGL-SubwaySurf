#pragma once
#include "Rendering/Texture.h"
#include "Rendering/VertexBufferObject.h"
#include "Rendering/Renderable.h"
#include "Geometry/Crystal.h"

// Sun renderable, vertices are defined in sun.csv, similar to crystal and heart
class CSun : public Renderable
{
public:
	CSun();
	~CSun();
	void Create(std::string directory, std::string filename);
	void Render() override;
	void RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) override;
	void Release();
private:
	void readFromCSV() override;

	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	GLuint m_instanceVBO;
	std::vector<vertex> m_sunVertices;
};
