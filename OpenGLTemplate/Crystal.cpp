#include "Common.h"
#include "Crystal.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define _USE_MATH_DEFINES

CCrystal::CCrystal()
{
}

CCrystal::~CCrystal()
{
}

void CCrystal::Create(string a_sDirectory, string a_sFilename)
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

	m_vbo.AddData(m_crystalVertices.data(), sizeof(vertex) * m_crystalVertices.size());

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Layout:
	// |position1;normal1;textureCoord1;color1|position2;normal2;textureCoord2;color2...position72;normal72;textureCoord72;color72|
	
	// Position has 3 elements, since we have interleaved layout stride will be equal to the size of the vertex, first attibute is at pointer 0
	GLsizei stride = sizeof(vertex);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Normals have 3 elements, Second attribute pointer starts directly after position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*) sizeof(glm::vec3));

	// Texture coords have 2 elements, third attribute pointer starts after position + normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*) (2 * sizeof(glm::vec3)));

	// Color has 3 elements, forth attribute pointer starts after position + normal + textureCoord (which is sizeof(vec2) instead of vec3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*) (2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCrystal::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_crystalVertices.size());
}

void CCrystal::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

void CCrystal::readFromCSV()
{
	m_crystalVertices.clear();

	ifstream f("crystal.csv");
	string line;

	while (getline(f, line)) {
		stringstream ss(line);
		string val;

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

		m_crystalVertices.push_back(newVertex);
	}
}
