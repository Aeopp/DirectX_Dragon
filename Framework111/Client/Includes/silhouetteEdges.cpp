#include "stdafx.h"
#include "silhouetteEdges.h"

SilhouetteEdges::SilhouetteEdges(IDirect3DDevice9* device, ID3DXMesh* mesh, ID3DXBuffer* adjBuffer)
{
}

SilhouetteEdges::~SilhouetteEdges()
{
}

void SilhouetteEdges::render()
{
}

bool SilhouetteEdges::createVertexDeclaration()
{
	return false;
}

void SilhouetteEdges::getFaceNormal(ID3DXMesh* mesh, DWORD faceIndex, _vector* faceNormal)
{
}

void SilhouetteEdges::getFaceNormals(ID3DXMesh* mesh, ID3DXMesh* adjBuffer, _vector* currentFaceNormal, _vector* adjFaceNormals[3], DWORD faceIndex)
{
}

void SilhouetteEdges::genEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer)
{
}

void SilhouetteEdges::genEdgeIndices(ID3DXMesh* mesh)
{
}
