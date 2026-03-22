#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a xz plane of a given size
class CPlane
{
public:
	CPlane();
	~CPlane();
	void Create(std::string sDirectory, std::string sFilename, float fWidth, float fHeight, float fTextureRepeat);
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	float m_width;
	float m_height;
};