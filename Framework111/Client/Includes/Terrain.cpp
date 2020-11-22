#include "stdafx.h"
#include "Terrain.h"
#include <fstream>
#include <cmath>

const DWORD Terrain::TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

Terrain::Terrain(IDirect3DDevice9* device,
	std::wstring heightmapFileName, 
	int numVertsPerRow, int numVertsPerCol,
	int cellSpacing, float heightScale)
{
	_device = device;
	_numVertsPerRow = numVertsPerRow;
	_numVertsPerCol = numVertsPerCol;
	_cellSpacing = cellSpacing;

	_numCellsPerRow = _numVertsPerRow -1 ;
	_numCellsPerCol = _numVertsPerCol - 1;

	_width = _numCellsPerRow * _cellSpacing;
	_depth = _numCellsPerCol * _cellSpacing;

	_numVertices = _numVertsPerRow * _numVertsPerCol;
	_numTriangles = _numCellsPerRow * _numCellsPerCol * 2;

	_heightScale = heightScale;

	if(!readRawFile(heightmapFileName))
	{
		::MessageBox(0, L"readRawFile - FAILED", 0, 0);
		::PostQuitMessage(0);	
	}

	for (int i = 0; i < _heightmap.size(); i++)
		_heightmap[i] *= heightScale;

	// compute the vertices
	if (!computeVertices())
	{
		::MessageBox(0, L"computeVertices - FAILED", 0, 0);
		::PostQuitMessage(0);
	}

	// compute the indices
	if (!computeIndices())
	{
		::MessageBox(0, L"computeIndices - FAILED", 0, 0);
		::PostQuitMessage(0);
	}
}

Terrain::~Terrain() noexcept
{
	d3d::Release(_vb);
	d3d::Release(_ib);
	d3d::Release(_tex);
}

int Terrain::getHeightmapEntry(int row, int col)
{
	return _heightmap[row * _numVertsPerRow + col];
}

void Terrain::setHeightmapEntry(int row, int col, int value)
{
	_heightmap[row * _numVertsPerRow + col] = value;
}

float Terrain::getHeight(float x, float z)
{
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = ((float)_width / 2.0f) + x;
	z = ((float)_depth / 2.0f) - z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= (float)_cellSpacing;
	z /= (float)_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	float col = ::floorf(x);
	float row = ::floorf(z);

	// get the heights of the quad we're in:
	// 
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D

	float A = getHeightmapEntry(row, col);
	float B = getHeightmapEntry(row, col + 1);
	float C = getHeightmapEntry(row + 1, col);
	float D = getHeightmapEntry(row + 1, col + 1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	if (dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		height = A + d3d::Lerp(0.0f, uy, dx) + d3d::Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + d3d::Lerp(0.0f, uy, 1.0f - dx) + d3d::Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
	
}

bool Terrain::loadTexture(std::wstring fileName)
{
	HRESULT hr = 0;

	hr = D3DXCreateTextureFromFile(
		_device,
		fileName.c_str(),
		&_tex);

	if (FAILED(hr))
		return false;

	return true;
	
}

bool Terrain::genTexture(D3DXVECTOR3* directionToLight)
{
	HRESULT hr = 0;

	int texWidth = _numCellsPerRow;
	int texHeight = _numCellsPerCol;

	hr = D3DXCreateTexture(
		_device,
		texWidth, texHeight,
		0,
		0,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED, &_tex);

	if (FAILED(hr))
		return false;

	D3DSURFACE_DESC textureDesc;
	_tex->GetLevelDesc(0, &textureDesc);

	if (textureDesc.Format != D3DFMT_X8R8G8B8)
		return false;

	D3DLOCKED_RECT lockedRect;
	_tex->LockRect(0, &lockedRect,0, 0);

	DWORD* imageData = (DWORD*)lockedRect.pBits;

	for(int i=0;i<texHeight;++i)
	{
		for(int j=0;j<texWidth ;++j)
		{
			D3DXCOLOR c;

			// get height of upper left vertex of quad.
			float height = (float)getHeightmapEntry(i, j) / _heightScale;

			if ((height) < 42.5f) 		 c = d3d::BEACH_SAND;
			else if ((height) < 85.0f)	 c = d3d::LIGHT_YELLOW_GREEN;
			else if ((height) < 127.5f) c = d3d::PUREGREEN;
			else if ((height) < 170.0f) c = d3d::DARK_YELLOW_GREEN;
			else if ((height) < 212.5f) c = d3d::DARKBROWN;
			else	                     c = d3d::WHITE;

			// fill locked data, note we divide the pitch by four because the
			// pitch is given in bytes and there are 4 bytes per DWORD.
			imageData[i * lockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	if (!lightTerrain(directionToLight))
	{
		::MessageBox(0, L"lightTerrain() - FAILED", 0, 0);
		return false;
	}

	hr = D3DXFilterTexture(
		_tex, 0, 0, D3DX_DEFAULT);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXFilterTexture() - FAILED", 0, 0);
		return false;
	}

	return true;
	
}

bool Terrain::draw(D3DXMATRIX* world, bool drawTris)
{
	HRESULT hr = 0;

	if (_device)
	{
		_device->SetTransform(D3DTS_WORLD, world);

		_device->SetStreamSource(0, _vb, 0, sizeof(TerrainVertex));
		_device->SetFVF(TerrainVertex::FVF);
		_device->SetIndices(_ib);

		_device->SetTexture(0, _tex);

		// turn off lighting since we're lighting it ourselves
		_device->SetRenderState(D3DRS_LIGHTING, false);

		hr = _device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			_numVertices,
			0,
			_numTriangles);

		_device->SetRenderState(D3DRS_LIGHTING, true);

		if (drawTris)
		{
			_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			hr = _device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				_numVertices,
				0,
				_numTriangles);

			_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

		if (FAILED(hr))
			return false;
	}

	return true;
	
}

bool Terrain::readRawFile(std::wstring fileName)
{
	// Restriction: RAW file dimensions must be >= to the
	// dimensions of the terrain.  That is a 128x128 RAW file
	// can only be used with a terrain constructed with at most
	// 128x128 vertices.

	// A height for each vertex
	std::vector<BYTE> in(_numVertices);

	std::ifstream inFile(fileName.c_str(), std::ios_base::binary);

	if (!inFile.is_open())
		return false;

	inFile.read(
		(char*)&in[0], // buffer
		in.size());// number of bytes to read into buffer

	inFile.close();

	// copy BYTE vector to int vector
	_heightmap.resize(_numVertices);

	for (int i = 0; i < in.size(); i++)
		_heightmap[i] = in[i];

	return true;
	
}

bool Terrain::computeVertices()
{
	HRESULT hr = 0;

	hr = _device->CreateVertexBuffer(
		_numVertices * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&_vb,
		0);

	if (FAILED(hr))
		return false;

	// coordinates to start generating vertices at
	int startX = -_width / 2;
	int startZ = _depth / 2;

	// coordinates to end generating vertices at
	int endX = _width / 2;
	int endZ = -_depth / 2;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	float uCoordIncrementSize = 1.0f / (float)_numCellsPerRow;
	float vCoordIncrementSize = 1.0f / (float)_numCellsPerCol;

	TerrainVertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int i = 0;
	for (int z = startZ; z >= endZ; z -= _cellSpacing)
	{
		int j = 0;
		for (int x = startX; x <= endX; x += _cellSpacing)
		{
			// compute the correct index into the vertex buffer and heightmap
			// based on where we are in the nested loop.
			int index = i * _numVertsPerRow + j;

			v[index] = TerrainVertex(
				(float)x,
				(float)_heightmap[index],
				(float)z,
				(float)j * uCoordIncrementSize,
				(float)i * vCoordIncrementSize);

			j++; // next column
		}
		i++; // next row
	}

	_vb->Unlock();

	return true;
}

bool Terrain::computeIndices()
{
	HRESULT hr = 0;
	hr = _device->CreateIndexBuffer(
		_numTriangles * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib, 0);

	if (FAILED(hr))
		return false;
	WORD* indices = 0;
	_ib->Lock(0, 0, (void**)&indices, 0);

	int baseIndex = 0;

	for(int i=0;i<_numCellsPerCol;++i)
	{
		for(int j=0;j<_numCellsPerRow;j++)
		{
			indices[baseIndex] = i * _numVertsPerRow + j;
			indices[baseIndex + 1] = i * _numVertsPerRow + j + 1;
			indices[baseIndex + 2] = (i + 1) * _numVertsPerRow + j;

			indices[baseIndex + 3] = (i + 1) * _numVertsPerRow + j;
			indices[baseIndex + 4] = i * _numVertsPerRow + j + 1;
			indices[baseIndex + 5] = (i + 1) * _numVertsPerRow + j + 1;

			// next quad
			baseIndex += 6;
		}
	}

	_ib->Unlock();

	return true;
	
}

bool Terrain::lightTerrain(D3DXVECTOR3* directionToLight)
{
	HRESULT hr = 0;

	D3DSURFACE_DESC textureDesc;
	_tex->GetLevelDesc(0, &textureDesc);

	if (textureDesc.Format != D3DFMT_X8R8G8B8)
		return false;

	D3DLOCKED_RECT lockedRect;
	_tex->LockRect(
		0,
		&lockedRect,
		0, 
		0);

	DWORD* imageData = (DWORD*)lockedRect.pBits;

	for(int i=0;i<textureDesc.Height;++i)
	{
		for(int j=0;j<textureDesc.Width;++j)
		{
			int index = i * lockedRect.Pitch / 4 + j;

			D3DXCOLOR c(imageData[index]);

			c *= computeShade(i, j, directionToLight);

			imageData[index] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	return true;
}

float Terrain::computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight)
{
	// get heights of three vertices on the quad
	float heightA = getHeightmapEntry(cellRow, cellCol);
	float heightB = getHeightmapEntry(cellRow, cellCol + 1);
	float heightC = getHeightmapEntry(cellRow + 1, cellCol);

	// build two vectors on the quad
	D3DXVECTOR3 u(_cellSpacing, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f, heightC - heightA, -_cellSpacing);

	// find the normal by taking the cross product of two
	// vectors on the quad.
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	float cosine = D3DXVec3Dot(&n, directionToLight);

	if (cosine < 0.0f)
		cosine = 0.0f;

	return cosine;
	
}
