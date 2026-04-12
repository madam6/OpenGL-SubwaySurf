/*

	Copyright 2011 Etay Meiri

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

#include <assert.h>
#include "OpenAssetImportMesh.h"

#pragma comment(lib, "lib/assimp.lib")

COpenAssetImportMesh::MeshEntry::MeshEntry()
{
    vbo = INVALID_OGL_VALUE;
    ibo = INVALID_OGL_VALUE;
    NumIndices  = 0;
    MaterialIndex = INVALID_MATERIAL;
};

COpenAssetImportMesh::MeshEntry::~MeshEntry()
{
    if (vbo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &vbo);

    if (ibo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &ibo);
}

void COpenAssetImportMesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                          const std::vector<unsigned int>& Indices)
{
    NumIndices = int(Indices.size());

	glGenBuffers(1, &vbo);
  	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

bool COpenAssetImportMesh::LoadFBX(const std::string& filename)
{
    m_Scene = m_Importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);

    if (!m_Scene || !m_Scene->mRootNode)
    {
        std::string errorMsg = "Failed to load FBX: " + filename + "\nReason: " + m_Importer.GetErrorString();
        MessageBox(NULL, errorMsg.c_str(), "Assimp Load Error", MB_ICONHAND);
        return false;
    }

    if (m_Scene->mNumAnimations > 0)
    {
        m_Animation = m_Scene->mAnimations[0];
    }

    for (unsigned int i = 0; i < m_Scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = m_Scene->mMeshes[i];
        for (unsigned int b = 0; b < mesh->mNumBones; ++b)
        {
            std::string boneName(mesh->mBones[b]->mName.data);
            if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
            {
                BoneInfo bi;
                bi.offsetMatrix = glm::transpose(glm::make_mat4(&mesh->mBones[b]->mOffsetMatrix.a1));
                m_BoneInfo.push_back(bi);
                m_BoneMapping[boneName] = m_NumBones++;
            }
        }
    }

    return InitFromScene(m_Scene, filename);
}

COpenAssetImportMesh::COpenAssetImportMesh()
{
}


COpenAssetImportMesh::~COpenAssetImportMesh()
{
    Clear();
}


void COpenAssetImportMesh::Clear()
{
    for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
        SAFE_DELETE(m_Textures[i]);
    }
	glDeleteVertexArrays(1, &m_vao);
}


bool COpenAssetImportMesh::Load(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
    
    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        MessageBox(NULL, Importer.GetErrorString(), "Error loading mesh model", MB_ICONHAND);
    }

    return Ret;
}

glm::vec3 COpenAssetImportMesh::InterpolatePosition(float time, const aiNodeAnim* node)
{
    if (node->mNumPositionKeys == 1)
    {
        return glm::vec3(node->mPositionKeys[0].mValue.x,
            node->mPositionKeys[0].mValue.y,
            node->mPositionKeys[0].mValue.z);
    }

    unsigned int positionIndex = 0;
    for (unsigned int i = 0; i < node->mNumPositionKeys - 1; i++)
    {
        if (time < (float)node->mPositionKeys[i + 1].mTime)
        {
            positionIndex = i;
            break;
        }
    }

    unsigned int nextPositionIndex = positionIndex + 1;
    const aiVectorKey& before = node->mPositionKeys[positionIndex];
    const aiVectorKey& after = node->mPositionKeys[nextPositionIndex];

    float deltaTime = (float)(after.mTime - before.mTime);
    float blendingFactor = (time - (float)before.mTime) / deltaTime;

    blendingFactor = std::max(0.0f, std::min(1.0f, blendingFactor));

    glm::vec3 startVec(before.mValue.x, before.mValue.y, before.mValue.z);
    glm::vec3 endVec(after.mValue.x, after.mValue.y, after.mValue.z);

    return glm::mix(startVec, endVec, blendingFactor);
}

glm::quat COpenAssetImportMesh::InterpolateRotation(float time, const aiNodeAnim* node)
{
    if (node->mNumRotationKeys == 1)
    {
        return glm::quat(node->mRotationKeys[0].mValue.w,
            node->mRotationKeys[0].mValue.x,
            node->mRotationKeys[0].mValue.y,
            node->mRotationKeys[0].mValue.z);
    }

    unsigned int rotationIndex = 0;
    for (unsigned int i = 0; i < node->mNumRotationKeys - 1; i++)
    {
        if (time < (float)node->mRotationKeys[i + 1].mTime)
        {
            rotationIndex = i;
            break;
        }
    }

    unsigned int nextRotationIndex = rotationIndex + 1;
    const aiQuatKey& before = node->mRotationKeys[rotationIndex];
    const aiQuatKey& after = node->mRotationKeys[nextRotationIndex];

    float deltaTime = (float)(after.mTime - before.mTime);
    float blendingFactor = (time - (float)before.mTime) / deltaTime;
    blendingFactor = std::max(0.0f, std::min(1.0f, blendingFactor));

    glm::quat startQuat(before.mValue.w, before.mValue.x, before.mValue.y, before.mValue.z);
    glm::quat endQuat(after.mValue.w, after.mValue.x, after.mValue.y, after.mValue.z);

    return glm::normalize(glm::slerp(startQuat, endQuat, blendingFactor));
}

glm::vec3 COpenAssetImportMesh::InterpolateScale(float time, const aiNodeAnim* node)
{
    if (node->mNumScalingKeys == 1)
    {
        return glm::vec3(node->mScalingKeys[0].mValue.x,
            node->mScalingKeys[0].mValue.y,
            node->mScalingKeys[0].mValue.z);
    }

    unsigned int scaleIndex = 0;
    for (unsigned int i = 0; i < node->mNumScalingKeys - 1; i++)
    {
        if (time < (float)node->mScalingKeys[i + 1].mTime)
        {
            scaleIndex = i;
            break;
        }
    }

    unsigned int nextScaleIndex = scaleIndex + 1;
    const aiVectorKey& before = node->mScalingKeys[scaleIndex];
    const aiVectorKey& after = node->mScalingKeys[nextScaleIndex];

    float deltaTime = (float)(after.mTime - before.mTime);
    float blendingFactor = (time - (float)before.mTime) / deltaTime;
    blendingFactor = std::max(0.0f, std::min(1.0f, blendingFactor));

    glm::vec3 startVec(before.mValue.x, before.mValue.y, before.mValue.z);
    glm::vec3 endVec(after.mValue.x, after.mValue.y, after.mValue.z);

    return glm::mix(startVec, endVec, blendingFactor);
}

bool COpenAssetImportMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{  
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

	glGenVertexArrays(1, &m_vao); 
	glBindVertexArray(m_vao);


    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}

void COpenAssetImportMesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
                 glm::vec2(pTexCoord->x, 1.0f-pTexCoord->y),
                 glm::vec3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int boneIndex = 0; boneIndex < paiMesh->mNumBones; ++boneIndex)
    {
        aiBone* bone = paiMesh->mBones[boneIndex];
        std::string boneName(bone->mName.data);

        int internalBoneID = m_BoneMapping[boneName];

        for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
        {
            unsigned int vertexId = bone->mWeights[weightIndex].mVertexId;
            float weight = bone->mWeights[weightIndex].mWeight;

            Vertices[vertexId].AddBoneData(internalBoneID, weight);
        }
    }

    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool COpenAssetImportMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("\\/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "\\";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

                std::string texturePath = Path.data;

                std::string::size_type lastSlash = texturePath.find_last_of("\\/");
                std::string justFilename = (lastSlash == std::string::npos) ? texturePath : texturePath.substr(lastSlash + 1);

                std::string FullPath = Dir + "/" + justFilename;

                m_Textures[i] = new CTexture();
                if (!m_Textures[i]->Load(FullPath, true)) {
                    MessageBox(NULL, FullPath.c_str(), "Error loading mesh texture", MB_ICONHAND);
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
                    m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

                    GLfloat maxAniso = 0.0f;
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
                    m_Textures[i]->SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        // Load a single colour texture matching the diffuse colour if no texture added
        if (!m_Textures[i])
        {
            // Fence hack
            std::string fallbackTexture = Dir + "/stone_fence_old_low_rock_BaseColor.png";

            m_Textures[i] = new CTexture();
            if (m_Textures[i]->Load(fallbackTexture, true))
            {
                DEBUG_MSG("Forced fallback texture load for: %s", fallbackTexture.c_str());
                m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
                m_Textures[i]->SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLfloat maxAniso = 0.0f;
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
                m_Textures[i]->SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
            }
            else
            {
                aiColor3D color(0.f, 0.f, 0.f);
                pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

                BYTE data[3];
                data[0] = (BYTE)(color[2] * 255);
                data[1] = (BYTE)(color[1] * 255);
                data[2] = (BYTE)(color[0] * 255);
                m_Textures[i]->CreateFromData(data, 1, 1, 24, GL_BGR, false);
            }
        }
    }

    return Ret;
}

void COpenAssetImportMesh::Render()
{
	glBindVertexArray(m_vao);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (const GLvoid*)32);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)48);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].ibo);


        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(0);
        }


        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
    }
}

void COpenAssetImportMesh::UpdateAnimation(float dt)
{
    if (!m_Animation) return;
    m_animationTime += dt * m_animationSpeed;

    float ticksPerSecond = (float)(m_Animation->mTicksPerSecond != 0 ? m_Animation->mTicksPerSecond : 25.0f);

    float timeInTicks = m_animationTime * ticksPerSecond;

    float animTime = fmod(timeInTicks, (float)m_Animation->mDuration);

    std::vector<glm::mat4> transforms(m_NumBones);

    BoneTransform(animTime, transforms);

    for (int i = 0; i < m_NumBones; ++i)
    {
        m_BoneInfo[i].finalTransform = transforms[i];
    }
}

void COpenAssetImportMesh::BoneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::function<void(const aiNode*, const glm::mat4&)> readNode;
    readNode = [&](const aiNode* node, const glm::mat4& parentTransform)
        {
            glm::mat4 nodeTransform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));

            
            if (m_Animation)
            {
                for (unsigned int i = 0; i < m_Animation->mNumChannels; i++)
                {
                    aiNodeAnim* anim = m_Animation->mChannels[i];
                    if (anim && std::string(anim->mNodeName.C_Str()) == node->mName.C_Str())
                    {
                        glm::vec3 translation = InterpolatePosition(timeInSeconds, anim);
                        glm::quat rotation = InterpolateRotation(timeInSeconds, anim);
                        glm::vec3 scale = InterpolateScale(timeInSeconds, anim);

                        glm::mat4 transMat = glm::translate(glm::mat4(1.0f), translation);
                        glm::mat4 rotMat = glm::mat4_cast(rotation);
                        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

                        nodeTransform = transMat * rotMat * scaleMat;
                        break;
                    }
                }
            }

            glm::mat4 globalTransform = parentTransform * nodeTransform;

            auto it = m_BoneMapping.find(node->mName.C_Str());
            if (it != m_BoneMapping.end())
            {
                int index = it->second;
                transforms[index] = globalTransform * m_BoneInfo[index].offsetMatrix;
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                readNode(node->mChildren[i], globalTransform);
            }
        };

    readNode(m_Scene->mRootNode, glm::mat4(1.f));
}
