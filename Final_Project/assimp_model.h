#pragma once
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "./common/shader.hpp"
#include "vertexBufferObject.h"
#include "texture.h"

class CMaterial
{
public:
	int iTexture;
};

class CMeshProperties
{
public:
	string m_Name;
	glm::vec3 m_scale;

	glm::vec3 m_center;
	glm::vec3 m_minExtents;
	glm::vec3 m_maxExtents;

	CMeshProperties();
	void updateExtents(const glm::vec3& pt);
	void updateExtents(const float& x, const float& y, const float& z);

};

class CAssimpModel
{
public:
	bool InitialEnvironment();
	bool LoadModelFromFile(char* sFilePath);

	static void FinalizeVBO();
	static void BindModelsVAO();

	void RenderModel();
	CAssimpModel();
private:
	bool bLoaded;
	static CVertexBufferObject vboModelData;
	static uint32_t uiVAO;
	static vector<CTexture> tTextures;
	vector<int> iMeshStartIndices;
	vector<int> iMeshSizes;
	vector<int> iMaterialIndices;
	int iNumMaterials;
	vector<CMeshProperties> m_meshProperties;

	GLuint m_ProgramID;

	// Get a handle for our "MVP" uniform
	GLuint m_MatrixID;
	GLuint m_ViewMatrixID;
	GLuint m_ModelMatrixID;

	// Get a handle for our buffers
	GLuint m_VertexPosition_modelspaceID;
	GLuint m_VertexUVID;
	GLuint m_VertexNormal_modelspaceID;

	//// Get a handle for our "myTextureSampler" uniform
	GLuint m_TextureID;

	// Get a handle for our "LightPosition" uniform
	GLuint m_LightID;

};
