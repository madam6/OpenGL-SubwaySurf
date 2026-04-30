#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

enum textureRetriever : uint8_t
{
	VALLEY_TEXTURE = 0,
	MOUNTAIN_TEXTURE,
	APEX_TEXTURE
};

// Class for generating a xz plane of a given size
class CPlane
{
public:
	CPlane();
	~CPlane();
	void Create(std::string sDirectory, std::string sFilename, float fWidth, float fHeight, float fTextureRepeat);
	void CreatePerlinTerrain(std::string sDirectory, const std::array<std::string, 3>& filenameTextures, float fWidth, float fHeight, float fTextureRepeat);
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_TextureLow;
	CTexture m_TextureHigh;
	CTexture m_TextureTop;
	std::string m_directory;
	std::string m_filename;
	float m_width;
	float m_height;
	int m_gridResolution;
	int m_numVertices;

	bool m_isPerlinTerrain{ false };
};