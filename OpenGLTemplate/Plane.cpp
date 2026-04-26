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
    m_texture.Load(directory + filename, true);

    m_directory = directory;
    m_filename = filename;

    // Set parameters for texturing using sampler object
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

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
void CPlane::CreatePerlinTerrain(std::string directory, std::string filename, float width, float height, float textureRepeat)
{
    m_width = width;
    m_height = height;
    m_isPerlinTerrain = true;

    // Load the texture
    m_texture.Load(directory + filename, true);

    m_directory = directory;
    m_filename = filename;

    // Set parameters for texturing using sampler object
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

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
    planeVertices.reserve(static_cast<int>((m_width / m_gridResolution) * (m_height / m_gridResolution)));

    for (int i = 0; i <= m_height; i += static_cast<int>(m_height / m_gridResolution))
    {
        for (int j = 0; j <= m_width; j += static_cast<int>(m_width / m_gridResolution))
        {
            planeVertices.emplace_back(-halfWidth + j, 0.f, -halfHeight + i);

            float u = (static_cast<float>(j) / m_width) * textureRepeat;
            float v = (static_cast<float>(i) / m_height) * textureRepeat;

            textureCoords.emplace_back(u, v);
        }
    }

    m_numVertices = planeVertices.size();

    int xStep = static_cast<int>(m_width / m_gridResolution);
    int vertsPerRow = (m_width / xStep) + 1;

    int numCols = vertsPerRow;
    int numRows = planeVertices.size() / vertsPerRow;

    int noiseGridSize = 15;
    std::vector<std::vector<glm::vec2>> latticePoints(noiseGridSize + 1, std::vector<glm::vec2>(noiseGridSize + 1, glm::vec2(0.0f, 1.f)));

    pairHash hasher;

    for (int i = 0; i < latticePoints.size(); i++)
    {
        for (int j = 0; j < latticePoints[0].size(); j++)
        {
            float value = static_cast<float>(hasher(std::make_pair(i, j))) / static_cast<float>(std::numeric_limits<size_t>::max());

            float angle = value * 2.0f * glm::pi<float>();

            latticePoints[i][j] = glm::rotate(latticePoints[i][j], angle);
        }
    }

    auto const perlinFadeFunc = [](float edge0, float edge1, float x)
        {
            float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;
        };


    for (int i = 0; i < m_numVertices; i++)
    {
        float fractionOfMapX = (float)(i % numCols) / numCols;
        float fractionOfMapY = (float)(i / numCols) / numRows;

        float noiseX = fractionOfMapX * noiseGridSize;
        float noiseY = fractionOfMapY * noiseGridSize;

        int x0 = static_cast<int>(floor(noiseX)) % noiseGridSize;
        int y0 = static_cast<int>(floor(noiseY)) % noiseGridSize;
        int x1 = (x0 + 1) % noiseGridSize;
        int y1 = (y0 + 1) % noiseGridSize;

        // Top left, top right, bottom left, bottom right
        std::vector<glm::vec2> gradientVectors{ latticePoints[y0][x0], latticePoints[y0][x1], latticePoints[y1][x0], latticePoints[y1][x1] };

        float fractionalX = noiseX - floor(noiseX);
        float fractionalY = noiseY - floor(noiseY);

        std::vector<glm::vec2> distanceVectors
        {
            glm::vec2(fractionalX, fractionalY), // Top left
            glm::vec2(fractionalX - 1.0f, fractionalY), // Top right
            glm::vec2(fractionalX, fractionalY - 1.0f), // Bottom left
            glm::vec2(fractionalX - 1.0f, fractionalY - 1.0f) // Bottom right
        };

        std::vector<float> gradients(4);

        for (int j = 0; j < gradients.size(); j++)
        {
            gradients[j] = glm::dot(distanceVectors[j], gradientVectors[j]);
        }

        float sx = perlinFadeFunc(0.f, 1.f, fractionalX);
        float sy = perlinFadeFunc(0.f, 1.f, fractionalY);

        float ix0 = glm::mix(gradients[0], gradients[1], sx);
        float ix1 = glm::mix(gradients[2], gradients[3], sx);

        planeVertices[i].y = glm::mix(ix0, ix1, sy) * 50.0f;
    }

    for (int i = 0; i < m_numVertices; i++)
    {
        glm::vec3 top = i - vertsPerRow < 0 ? planeVertices[i] : planeVertices[i - vertsPerRow];
        glm::vec3 bottom = i + vertsPerRow < planeVertices.size() ? planeVertices[i + vertsPerRow] : planeVertices[i];

        bool isLeftEdge = (i % vertsPerRow == 0);
        bool isRightEdge = (i % vertsPerRow == vertsPerRow - 1);

        glm::vec3 left = isLeftEdge ? planeVertices[i] : planeVertices[i - 1];
        glm::vec3 right = isRightEdge ? planeVertices[i] : planeVertices[i + 1];

        glm::vec3 horizontalSlope = left - right;
        glm::vec3 verticalSlop = top - bottom;

        normals.push_back(glm::normalize(glm::cross(horizontalSlope, verticalSlop)));
    }

    m_numVertices = 0;

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
                m_vbo.AddData(&normals[index], sizeof(glm::vec3));
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
	m_texture.Bind();

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
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}