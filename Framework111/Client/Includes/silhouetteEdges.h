#pragma once
#include "d3dUtility.h"

struct EdgeVertex
{
	_vector position;
	_vector normal;
	_vector faceNormal1 ;
	_vector faceNormal2;
};

struct MeshVertex
{
	_vector position;
	_vector normal;
	static const DWORD FVF;
};

class SilhouetteEdges
{
public :
	SilhouetteEdges(
		IDirect3DDevice9* device,
		ID3DXMesh* mesh,
		ID3DXBuffer* adjBuffer);

	~SilhouetteEdges();

	void render();
private:
	IDirect3DDevice9* _device;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9* _ib;
	IDirect3DVertexDeclaration9* _decl;
	
	UINT _numVerts;
	UINT _numFaces;
private:
	bool createVertexDeclaration();
	
	void getFaceNormal(
		ID3DXMesh* mesh,
		DWORD faceIndex,
		_vector* faceNormal);

	void getFaceNormals(
		ID3DXMesh* mesh,
		ID3DXMesh* adjBuffer,
		_vector* currentFaceNormal,
		_vector* adjFaceNormals[3],
		DWORD faceIndex);

	void genEdgeVertices(
		ID3DXMesh* mesh,
		ID3DXBuffer* adjBuffer);

	void genEdgeIndices
	(
		ID3DXMesh* mesh);

};
