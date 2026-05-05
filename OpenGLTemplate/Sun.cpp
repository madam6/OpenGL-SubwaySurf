#include "Common.h"
#include "Sun.h"
#include "Crystal.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

CSun::CSun() {}
CSun::~CSun() {}

void CSun::Create(std::string a_sDirectory, std::string a_sFilename)
{
	m_texture.Load(a_sDirectory + a_sFilename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	readFromCSV();

	m_vbo.AddData(m_sunVertices.data(), sizeof(vertex) * m_sunVertices.size());
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = sizeof(vertex);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, textureCoord));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));

	glBindVertexArray(m_vao);
}

void CSun::RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) {}

void CSun::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_sunVertices.size());
}

void CSun::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

void CSun::readFromCSV()
{
	m_sunVertices.clear();
	std::ifstream f("sun.csv");
	std::string line;

	while (getline(f, line)) 
	{
		std::stringstream ss(line);
		std::string val;
		vertex newVertex;
		float x, y, z;

		getline(ss, val, ','); x = static_cast<float>(stof(val));
		getline(ss, val, ','); y = static_cast<float>(stof(val));
		getline(ss, val, ','); z = static_cast<float>(stof(val));
		newVertex.position = glm::vec3(x, y, z);

		getline(ss, val, ','); x = static_cast<float>(stof(val));
		getline(ss, val, ','); y = static_cast<float>(stof(val));
		newVertex.textureCoord = glm::vec2(x, y);

		getline(ss, val, ','); x = static_cast<float>(stof(val));
		getline(ss, val, ','); y = static_cast<float>(stof(val));
		getline(ss, val, ','); z = static_cast<float>(stof(val));
		newVertex.normal = glm::vec3(x, y, z);

		newVertex.color = glm::vec3(1, 1, 1);
		m_sunVertices.push_back(newVertex);
	}
}
