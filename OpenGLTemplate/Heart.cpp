#include "Common.h"
#include "Heart.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

CHeart::CHeart() {}

CHeart::~CHeart() {}

void CHeart::Create(std::string a_sDirectory, std::string a_sFilename)
{
	m_texture.Load(a_sDirectory + a_sFilename);
	m_directory = a_sDirectory;
	m_filename = a_sFilename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	readFromCSV();

	m_vbo.AddData(m_heartVertices.data(), sizeof(vertex) * m_heartVertices.size());
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
	glGenBuffers(1, &m_instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);

	glBufferData(GL_ARRAY_BUFFER, 500 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
		glVertexAttribDivisor(4 + i, 1);
	}

	glBindVertexArray(0);
}

void CHeart::RenderInstanced(const std::vector<glm::mat4>& instanceMatrices)
{
	if (instanceMatrices.empty()) return;

	glBindVertexArray(m_vao);
	m_texture.Bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data());

	glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei)m_heartVertices.size(), (GLsizei)instanceMatrices.size());
	glBindVertexArray(0);
}

void CHeart::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_heartVertices.size());
}

void CHeart::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

void CHeart::readFromCSV()
{
	m_heartVertices.clear();

	std::ifstream f("heart.csv");
	std::string line;

	while (getline(f, line)) 
	{
		std::stringstream ss(line);
		std::string val;

		vertex newVertex;

		float x, y, z;

		getline(ss, val, ',');
		x = static_cast<float>(stof(val));

		getline(ss, val, ',');
		y = static_cast<float>(stof(val));

		getline(ss, val, ',');
		z = static_cast<float>(stof(val));

		newVertex.position = glm::vec3(x, y, z);


		getline(ss, val, ',');
		x = static_cast<float>(stof(val));

		getline(ss, val, ',');
		y = static_cast<float>(stof(val));

		newVertex.textureCoord = glm::vec2(x, y);


		getline(ss, val, ',');
		x = static_cast<float>(stof(val));

		getline(ss, val, ',');
		y = static_cast<float>(stof(val));

		getline(ss, val, ',');
		z = static_cast<float>(stof(val));

		newVertex.normal = glm::vec3(x, y, z);

		// We do not really care about color
		newVertex.color = glm::vec3(1, 1, 1);
		m_heartVertices.push_back(newVertex);
	}
}