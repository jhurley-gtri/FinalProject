#include "common_header.h"

#include "assimp_model.h"

#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

CVertexBufferObject CAssimpModel::vboModelData;
UINT CAssimpModel::uiVAO;
vector<CTexture> CAssimpModel::tTextures;


/*-----------------------------------------------

Name:	

Params:	
Result: 
/*---------------------------------------------*/

CMeshProperties::CMeshProperties() : 
    m_Name(""), 
    m_scale(glm::vec3(1.0, 1.0, 1.0)), 
    m_center(glm::vec3(0.0, 0.0, 0.0)), 
    m_minExtents(glm::vec3(DBL_MAX, DBL_MAX, DBL_MAX)), 
    m_maxExtents(glm::vec3(DBL_MIN, DBL_MIN, DBL_MIN))
{}

void CMeshProperties::updateExtents(const float& x, const float& y, const float& z)
{
    if (x < m_minExtents.x) m_minExtents.x = x;
    if (y < m_minExtents.y) m_minExtents.y = y;
    if (z < m_minExtents.z) m_minExtents.z = z;

    if (x > m_maxExtents.x) m_maxExtents.x = x;
    if (y > m_maxExtents.y) m_maxExtents.y = y;
    if (z > m_maxExtents.z) m_maxExtents.z = z;

    m_center.x = m_maxExtents.x - m_minExtents.x;
    m_center.y = m_maxExtents.y - m_minExtents.y;
    m_center.z = m_maxExtents.z - m_minExtents.z;

}

void CMeshProperties::updateExtents(const glm::vec3& pt)
{
    if (pt.x < m_minExtents.x) m_minExtents.x = pt.x;
    if (pt.y < m_minExtents.y) m_minExtents.y = pt.y;
    if (pt.z < m_minExtents.z) m_minExtents.z = pt.z;

    if (pt.x > m_maxExtents.x) m_maxExtents.x = pt.x;
    if (pt.y > m_maxExtents.y) m_maxExtents.y = pt.y;
    if (pt.z > m_maxExtents.z) m_maxExtents.z = pt.z;

    m_center.x = m_maxExtents.x - m_minExtents.x;
    m_center.y = m_maxExtents.y - m_minExtents.y;
    m_center.z = m_maxExtents.z - m_minExtents.z;

}

/*-----------------------------------------------

Name:	GetDirectoryPath

Params:	sFilePath - guess ^^

Result: Returns directory name only from filepath.

/*---------------------------------------------*/

string GetDirectoryPath(string sFilePath)
{
    // Get directory path
    string sDirectory = "";
    RFOR(i, ESZ(sFilePath) - 1)if (sFilePath[i] == '\\' || sFilePath[i] == '/')
    {
        sDirectory = sFilePath.substr(0, i + 1);
        break;
    }
    return sDirectory;
}

CAssimpModel::CAssimpModel()
{
    bLoaded = false;
}

/*-----------------------------------------------

Name:	LoadModelFromFile

Params:	sFilePath - guess ^^

Result: Loads model using Assimp library.

/*---------------------------------------------*/

bool CAssimpModel::LoadModelFromFile(char* sFilePath)
{
    if (vboModelData.GetBufferID() == 0)
    {
        vboModelData.CreateVBO();
        tTextures.reserve(50);
    }
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(sFilePath,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (!scene)
    {
        return false;
    }

    const int iVertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);

    int iTotalVertices = 0;

    // Determine the extents of the mesh
    m_meshProperties.resize(scene->mNumMeshes);
    FOR(i, scene->mNumMeshes)
    {
        aiMesh* mesh = scene->mMeshes[i];
        if (!mesh->HasTextureCoords(0))
        {
            continue;
        }
        int iMeshVertices = mesh->mNumVertices;
        FOR(j, iMeshVertices)
        {
            m_meshProperties[i].updateExtents(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
        }
    }
    // Load in the mesh vertex information
    FOR(i, scene->mNumMeshes)
    {
        aiMesh* mesh = scene->mMeshes[i];
        if (!mesh->HasTextureCoords(0))
        {
            continue;
        }
        int iMeshFaces = mesh->mNumFaces;

        iMaterialIndices.push_back(mesh->mMaterialIndex);
        int iSizeBefore = vboModelData.GetCurrentSize();
        iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);
        FOR(j, iMeshFaces)
        {
            const aiFace& face = mesh->mFaces[j];
            FOR(k, 3)
            {
                aiVector3D pos = mesh->mVertices[face.mIndices[k]];
                aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
                aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
                vboModelData.AddData(&pos, sizeof(aiVector3D));
                vboModelData.AddData(&uv, sizeof(aiVector2D));
                vboModelData.AddData(&normal, sizeof(aiVector3D));
            }
        }
        int iMeshVertices = mesh->mNumVertices;
        iTotalVertices += iMeshVertices;
        iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
    }
    iNumMaterials = scene->mNumMaterials;

    // Load in the texture information
    vector<int> materialRemap(iNumMaterials);

    FOR(i, iNumMaterials)
    {
        const aiMaterial* material = scene->mMaterials[i];
        int a = 5;
        int texIndex = 0;
        aiString path;  // filename

        if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
        {
            string sDir = GetDirectoryPath(sFilePath);
            string sTextureName = path.data;
            string sFullPath = sDir + sTextureName;
            int iTexFound = -1;
            FOR(j, ESZ(tTextures))if (sFullPath == tTextures[j].GetPath())
            {
                iTexFound = j;
                break;
            }
            if (iTexFound != -1)materialRemap[i] = iTexFound;
            else
            {
                CTexture tNew;
                tNew.LoadTexture2D(sFullPath, true);
                materialRemap[i] = ESZ(tTextures);
                tTextures.push_back(tNew);
            }
        }
    }

    FOR(i, ESZ(iMeshSizes))
    {
        int iOldIndex = iMaterialIndices[i];
        iMaterialIndices[i] = materialRemap[iOldIndex];
    }

    return bLoaded = true;
}

/*-----------------------------------------------

Name:	FinalizeVBO

Params: none

Result: Uploads all loaded model data in one global
        models' VBO.

/*---------------------------------------------*/

void CAssimpModel::FinalizeVBO()
{
    glGenVertexArrays(1, &uiVAO);
    glBindVertexArray(uiVAO);
    vboModelData.BindVBO();
    vboModelData.UploadDataToGPU(GL_STATIC_DRAW);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D), 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D), (void*)sizeof(aiVector3D));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D), (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));
}

/*-----------------------------------------------

Name:	BindModelsVAO

Params: none

Result: Binds VAO of models with their VBO.

/*---------------------------------------------*/

void CAssimpModel::BindModelsVAO()
{
    glBindVertexArray(uiVAO);
}

/*-----------------------------------------------

Name:	RenderModel

Params: none

Result: Guess what it does ^^.

/*---------------------------------------------*/

void CAssimpModel::RenderModel()
{
    if (!bLoaded)
        return;
    int iNumMeshes = ESZ(iMeshSizes);
    FOR(i, iNumMeshes)
    {
        int iMatIndex = iMaterialIndices[i];
        if (tTextures.size() > iMatIndex)
            tTextures[iMatIndex].BindTexture();
        // Update your model matrix 
        // Look for collisions and 
        glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
    }
}