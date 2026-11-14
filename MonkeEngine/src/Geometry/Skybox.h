#pragma once

#include "Rendering/Texture.h"
#include "Rendering/VertexBufferObject.h"
#include "Rendering/Cubemap.h"

// This is a class for creating and rendering a skybox
class CSkybox
{
public:
	CSkybox();
	~CSkybox();
	void Create(float size);
	void Render(int textureUnit);
	void Release();

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CCubemap m_cubemapTexture;
	
};