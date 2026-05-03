#include "Common.h"
#include "FrameBufferObject.h"
#include "./include/glm/gtc/matrix_transform.hpp"

CFrameBufferObject::CFrameBufferObject()
{
	m_uiFramebuffer = 0;
	m_uiDepthTexture = 0;
	m_uiColourTexture = 0;
}

CFrameBufferObject::~CFrameBufferObject()
{
	Release();
}

bool CFrameBufferObject::Create(int a_iWidth, int a_iHeight)
{
	if (m_uiFramebuffer != 0) return false;

	glGenFramebuffers(1, &m_uiFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFramebuffer);

	glGenTextures(1, &m_uiDepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiDepthTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, a_iWidth, a_iHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_uiDepthTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	m_iWidth = a_iWidth;
	m_iHeight = a_iHeight;

	bool status = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return status;
}

void CFrameBufferObject::Bind(bool bSetFullViewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFramebuffer);
	if (bSetFullViewport) glViewport(0, 0, m_iWidth, m_iHeight);

	glClear(GL_DEPTH_BUFFER_BIT);
}

void CFrameBufferObject::BindTexture(int iTextureUnit)
{
	// Not used for depth-only shadow maps
}

void CFrameBufferObject::BindDepth(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_uiDepthTexture);
}

void CFrameBufferObject::Release()
{
	if (m_uiFramebuffer)
	{
		glDeleteFramebuffers(1, &m_uiFramebuffer);
		m_uiFramebuffer = 0;
	}

	glDeleteTextures(1, &m_uiDepthTexture);
}

int CFrameBufferObject::GetWidth()
{
	return m_iWidth;
}

int CFrameBufferObject::GetHeight()
{
	return m_iHeight;
}

void CFrameBufferObject::SetSamplerObjectParameter(GLenum parameter, GLenum value)
{
}

void CFrameBufferObject::SetSamplerObjectParameterf(GLenum parameter, float value)
{
}