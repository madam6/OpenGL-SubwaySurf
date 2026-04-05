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

#define MAX_INFLUENCE 4

struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;
    int m_boneIDs[MAX_INFLUENCE];
    float m_weights[MAX_INFLUENCE];

    Vertex() {}

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
        ResetBones();
    }

    void ResetBones()
    {
        for (int i = 0; i < MAX_INFLUENCE; i++)
        {
            m_boneIDs[i] = 0;
            m_weights[i] = 0.0f;
        }
    }

    void AddBoneData(int boneID, float weight)
    {
        for (int i = 0; i < MAX_INFLUENCE; i++)
        {
            if (m_weights[i] == 0.0f)
            {
                m_boneIDs[i] = boneID;
                m_weights[i] = weight;
                return;
            }
        }
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
    void SetAnimationSpeed(float speed) { m_animationSpeed = speed; }
    float GetAnimationSpeed() const { return m_animationSpeed; }
private:
    glm::vec3 InterpolatePosition(float time, const aiNodeAnim* node);
    glm::quat InterpolateRotation(float time, const aiNodeAnim* node);
    glm::vec3 InterpolateScale(float time, const aiNodeAnim* node);

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



