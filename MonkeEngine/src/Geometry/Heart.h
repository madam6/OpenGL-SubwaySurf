#pragma once
#include "Rendering/Texture.h"
#include "Rendering/VertexBufferObject.h"
#include "Rendering/Renderable.h"
#include "Geometry/Crystal.h"

// Basic shape of the heart defined in heart.csv similarly to crystal
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