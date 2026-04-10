#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"
#include "Renderable.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoord;
	glm::vec3 color;
};

class CCrystal : public Renderable
{
public:
	CCrystal();
	~CCrystal();
	void Create(std::string directory, std::string front);
	void Render();
	void RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) override;
	void Release();
private:
	// Reads from CSV version of table created in report for task 1
	void readFromCSV();


	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	GLuint m_instanceVBO;
	
	std::vector<vertex> m_crystalVertices;
};