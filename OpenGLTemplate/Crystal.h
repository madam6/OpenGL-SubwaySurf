#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoord;
	glm::vec3 color;
};

class CCrystal
{
public:
	CCrystal();
	~CCrystal();
	void Create(std::string directory, std::string front);
	void Render();
	void Release();
private:
	// Reads from CSV version of table created in report for task 1
	void readFromCSV();


	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	
	std::vector<vertex> m_crystalVertices;
};