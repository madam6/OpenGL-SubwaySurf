#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"
#include "Renderable.h"
#include "Crystal.h"

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
