#pragma once
#include <vector>
/********************************

Class:		CVertexBufferObject

Purpose:	Wraps OpenGL vertex buffer
			object.

********************************/
using namespace std;

class CVertexBufferObject
{
public:
	void CreateVBO(int a_iSize = 0);
	void DeleteVBO();

	void* MapBufferToMemory(int iUsageHint);
	void* MapSubBufferToMemory(int iUsageHint, uint32_t uiOffset, uint32_t uiLength);
	void UnmapBuffer();

	void BindVBO(int a_iBufferType = GL_ARRAY_BUFFER);
	void UploadDataToGPU(int iUsageHint);
	
	void AddData(void* ptrData, uint32_t uiDataSize);

	void* GetDataPointer();
	uint32_t GetBufferID();

	int GetCurrentSize();

	CVertexBufferObject();

private:
	uint32_t uiBuffer;
	int iSize;
	int iCurrentSize;
	int iBufferType;
	vector<uint8_t> data;

	bool bDataUploaded;
};