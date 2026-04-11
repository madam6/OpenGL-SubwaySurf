#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"
#include "Renderable.h"
#include "Crystal.h"

class CHeart : public Renderable
{
public:
	CHeart();
	~CHeart();
	void Create(std::string directory, std::string filename);
	void Render();
	void RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) override;
	void Release();

private:
	void readFromCSV() override;

	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	GLuint m_instanceVBO;

	std::vector<vertex> m_heartVertices;
};