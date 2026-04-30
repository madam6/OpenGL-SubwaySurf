#include "Common.h"
#include "Plane.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CPlane::CPlane()
	: m_gridResolution(200)
{}

CPlane::~CPlane()
{}


struct pairHash
{
	template<class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& pair) const
	{
		auto h1 = std::hash<T1>{}(pair.first);
		auto h2 = std::hash<T2>{}(pair.second);

		return h1 ^ (h2 << 1);
	}
};

// Create the plane, including its geometry, texture mapping, normal, and colour
void CPlane::Create(std::string directory, std::string filename, float width, float height, float textureRepeat)
{
    m_width = width;
    m_height = height;
    m_isPerlinTerrain = false;
    // Load the texture
    m_TextureLow.Load(directory + filename, true);

    m_directory = directory;
    m_filename = filename;

    // Set parameters for texturing using sampler object
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    m_TextureLow.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create a VBO
    m_vbo.Create();
    m_vbo.Bind();

    float halfWidth = m_width / 2.0f;
    float halfHeight = m_height / 2.0f;

    // Vertex positions
    glm::vec3 planeVertices[4] =
    {
        glm::vec3(-halfWidth, 0.0f, -halfHeight),
        glm::vec3(-halfWidth, 0.0f, halfHeight),
        glm::vec3(halfWidth, 0.0f, -halfHeight),
        glm::vec3(halfWidth, 0.0f, halfHeight),
    };

    // Texture coordinates
    glm::vec2 planeTexCoords[4] =
    {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, textureRepeat),
        glm::vec2(textureRepeat, 0.0f),
        glm::vec2(textureRepeat, textureRepeat)
    };

    // Plane normal
    glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    // Put the vertex attributes in the VBO
    for (unsigned int i = 0; i < 4; i++) {
        m_vbo.AddData(&planeVertices[i], sizeof(glm::vec3));
        m_vbo.AddData(&planeTexCoords[i], sizeof(glm::vec2));
        m_vbo.AddData(&planeNormal, sizeof(glm::vec3));
    }


    // Upload the VBO to the GPU
    m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

    // Set the vertex attribute locations
    GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Create the plane, including its geometry, texture mapping, normal, and colour
void CPlane::CreatePerlinTerrain(std::string directory, const std::array<std::string, 3>& filenameTextures, float width, float height, float textureRepeat)
{
    m_width = width;
    m_height = height;
    m_isPerlinTerrain = true;

    // Load the texture
    m_TextureLow.Load(directory + filenameTextures.at(textureRetriever::VALLEY_TEXTURE), true);
    m_TextureHigh.Load(directory + filenameTextures.at(textureRetriever::MOUNTAIN_TEXTURE), true);
    m_TextureTop.Load(directory + filenameTextures.at(textureRetriever::APEX_TEXTURE), true);

    m_directory = directory;
    m_filename = filenameTextures.at(textureRetriever::VALLEY_TEXTURE);

    // Set parameters for texturing using sampler object
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_TextureLow.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    m_TextureHigh.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_TextureHigh.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_TextureHigh.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_TextureHigh.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    m_TextureTop.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_TextureTop.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_TextureTop.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_TextureTop.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    m_TextureLow.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    m_TextureHigh.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    m_TextureTop.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create a VBO
    m_vbo.Create();
    m_vbo.Bind();

    float halfWidth = m_width / 2.0f;
    float halfHeight = m_height / 2.0f;

    std::vector<glm::vec3> planeVertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    for (int i = 0; i <= m_height; i += static_cast<int>(m_height / m_gridResolution))
    {
        for (int j = 0; j <= m_width; j += static_cast<int>(m_width / m_gridResolution))
        {
            planeVertices.emplace_back(-halfWidth + j, 0.0f, -halfHeight + i);
            float u = (static_cast<float>(j) / m_width) * textureRepeat;
            float v = (static_cast<float>(i) / m_height) * textureRepeat;

            textureCoords.emplace_back(u, v);
        }
    }

    m_numVertices = 0;
    int xStep = static_cast<int>(m_width / m_gridResolution);
    int vertsPerRow = (m_width / xStep) + 1;
    int numCols = vertsPerRow;
    int numRows = planeVertices.size() / vertsPerRow;
    glm::vec3 normal(0.f, 1.f, 0.f);

    for (int z = 0; z < numRows-1; z++)
    {
        for (int x = 0; x < numCols-1; x++)
        {
            int topLeft = z * vertsPerRow + x;
            int bottomLeft = (z + 1) * vertsPerRow + x;
            int topRight = topLeft + 1;
            int bottomRight = bottomLeft + 1;

            auto pushVertex = [&](int index) 
            {
                m_vbo.AddData(&planeVertices[index], sizeof(glm::vec3));
                m_vbo.AddData(&textureCoords[index], sizeof(glm::vec2));
                m_vbo.AddData(&normal, sizeof(glm::vec3));
                m_numVertices++;
            };

            pushVertex(topLeft);
            pushVertex(bottomLeft);
            pushVertex(topRight);

            pushVertex(topRight);
            pushVertex(bottomLeft);
            pushVertex(bottomRight);
        }
    }

    // Upload the VBO to the GPU
    m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

    // Set the vertex attribute locations
    GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}
// Render the plane as a triangle strip
void CPlane::Render()
{
	glBindVertexArray(m_vao);
	m_TextureLow.Bind(0);
    m_TextureHigh.Bind(1);
    m_TextureTop.Bind(2);

    if (m_isPerlinTerrain)
    {
        glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
    }
    else
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

// Release resources
void CPlane::Release()
{
	m_TextureLow.Release();
    m_TextureHigh.Release();
    m_TextureTop.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}