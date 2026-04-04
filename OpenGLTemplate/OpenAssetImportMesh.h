/*

	Copyright 2011 Etay Meiri

	// Modified by Dr Greg Slabaugh to work with OpenGL template

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <map>
#include <vector>
#include "include/gl/glew.h"
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <PostProcess.h> // Post processing flags

#include "Common.h"
#include "Texture.h"

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }


struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }
};

struct BoneInfo
{
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransform;
};

class COpenAssetImportMesh
{
public:
    COpenAssetImportMesh();
    ~COpenAssetImportMesh();

    bool Load(const std::string& Filename);
    bool LoadFBX(const std::string& filename);

    void Render();
    void UpdateAnimation(float dt);
    void BoneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms);

    int GetNumBones() const { return m_NumBones; }
    const std::vector<BoneInfo>& GetBoneInfo() const { return m_BoneInfo; }

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();
	

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
                  const std::vector<unsigned int>& Indices);
        GLuint vbo;
        GLuint ibo;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    float m_animationTime = 0.f;
    float m_animationSpeed = 1.f;

    std::vector<MeshEntry> m_Entries;
    std::vector<CTexture*> m_Textures;

    std::map<std::string, int> m_BoneMapping;
    std::vector<BoneInfo> m_BoneInfo;
    aiAnimation* m_Animation = nullptr;
    const aiScene* m_Scene = nullptr;
    int m_NumBones = 0;

    Assimp::Importer m_Importer;
	GLuint m_vao;
};



