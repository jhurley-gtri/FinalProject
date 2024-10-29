#pragma once

#include "./common/shader.hpp"
#include "vertexBufferObject.h"
#include "texture.h"

class CMaterial
{
public:
	int iTexture;
};

class CAssimpModel
{
public:
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
};
