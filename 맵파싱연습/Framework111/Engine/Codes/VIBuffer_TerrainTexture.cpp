#include "VIBuffer_TerrainTexture.h"

USING(Engine)

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(
	LPDIRECT3DDEVICE9 pDevice,
	_uint iVertexCountX,
	_uint iVertexCountZ,
	float fVertexInterval/* = 1.f*/)
	: CVIBuffer(pDevice)
	, m_iVertexCountX(iVertexCountX)
	, m_iVertexCountZ(iVertexCountZ)
	, m_fVertexInterval(fVertexInterval)
{
}

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(const CVIBuffer_TerrainTexture & other)
	: CVIBuffer(other)
	, m_iVertexCountX(other.m_iVertexCountX)
	, m_iVertexCountZ(other.m_iVertexCountZ)
	, m_fVertexInterval(other.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainTexture::ReadyComponentPrototype()
{
	m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iFVF = FVF_VTX_TEXTURE;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::ReadyComponentPrototype()))
		return E_FAIL;

	VTX_TEXTURE* pVertex = nullptr;
	_uint* pPixels = LoadHeightMap(L"../Resources/Terrain/Height.bmp");
	if (nullptr == pPixels)
	{
		PRINT_LOG(L"Warning", L"Failed To LoadHeightMap");
		return E_FAIL;
	}

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_uint iIndex = 0;

	for (_uint z = 0; z < m_iVertexCountZ; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX; ++x)
		{
			iIndex = z * m_iVertexCountX + x;

			if (m_iVertexCount <= iIndex)
				continue;
			
			float fBlue = float(pPixels[iIndex] & 0x000000ff);
			pVertex[iIndex].vPosition = D3DXVECTOR3(x * m_fVertexInterval, fBlue / 100.f, z * m_fVertexInterval);
			pVertex[iIndex].vUV = D3DXVECTOR2(x / (m_iVertexCountX - 1.f), 1.f - (z / (m_iVertexCountZ - 1.f)));
		}
	}

	m_pVertices = new VTX_TEXTURE[m_iVertexCount];
	memcpy(m_pVertices, pVertex, sizeof(VTX_TEXTURE) * m_iVertexCount);

	m_pVB->Unlock();

	SafeDeleteArray(pPixels);

	INDEX16* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);	

	_uint iTriIndex = 0;

	for (_uint z = 0; z < m_iVertexCountZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX - 1; ++x)
		{
			iIndex = z * m_iVertexCountX + x;

			if (m_iVertexCount <= iIndex)
				continue;	

			pIndex[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndex[iTriIndex]._2 = iIndex + m_iVertexCountX + 1;
			pIndex[iTriIndex++]._3 = iIndex + 1;

			pIndex[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndex[iTriIndex]._2 = iIndex + 1;
			pIndex[iTriIndex++]._3 = iIndex;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TerrainTexture::ReadyComponent(void * pArg)
{
	if (FAILED(CVIBuffer::ReadyComponent(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_TerrainTexture::Render_VIBuffer()
{
	if (FAILED(CVIBuffer::Render_VIBuffer()))
		return E_FAIL;

	/* 장치에게 인덱스버퍼 셋 */
	if (FAILED(m_pDevice->SetIndices(m_pIB)))
		return E_FAIL;

	/* 인덱스를 이용해서 도형을 그리는 함수 */
	return m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount);
}

_bool CVIBuffer_TerrainTexture::IsCollision(_vector* pTargetPosition)
{
	if (nullptr == pTargetPosition)
		return false;

	VTX_TEXTURE* pVertices = (VTX_TEXTURE*)m_pVertices;

	/*if (pVertices[0].vPosition.x > pTargetPosition->x ||
		pVertices[m_iVertexCountX - 1].vPosition.x < pTargetPosition->x)
		return false;

	if (pVertices[0].vPosition.z > pTargetPosition->z ||
		pVertices[m_iVertexCountX - 1].vPosition.z < pTargetPosition->z)
		return false;*/

	_uint iRow = _uint(pTargetPosition->z / m_fVertexInterval);
	_uint iCol = _uint(pTargetPosition->x / m_fVertexInterval);
	_uint iIndex = iRow * m_iVertexCountX + iCol;
	if (m_iVertexCount <= iIndex)
		return false;

	D3DXPLANE	plane;

	float fRatioX =  pTargetPosition->x - 
		pVertices[iIndex + m_iVertexCountX].vPosition.x;
	float fRatioZ = pVertices[iIndex + m_iVertexCountX].vPosition.z -
		pTargetPosition->z;		

	if (fRatioX > fRatioZ) /* 오른쪽 삼각형을 밟고 있는 경우*/
	{
		D3DXPlaneFromPoints(
			&plane,
			&pVertices[iIndex + m_iVertexCountX].vPosition,
			&pVertices[iIndex + m_iVertexCountX + 1].vPosition,
			&pVertices[iIndex + 1].vPosition);
	}
	else /* 왼쪽 삼각형을 밟고 있는 경우 */
	{
		D3DXPlaneFromPoints(
			&plane,
			&pVertices[iIndex + m_iVertexCountX].vPosition,
			&pVertices[iIndex + 1].vPosition,
			&pVertices[iIndex].vPosition);
	}

	/*ax + by + cz + d = 0
	y = -(ax + cz + d) / b*/
	float x = pTargetPosition->x;	
	float z = pTargetPosition->z;

	if (0.f == plane.b)
		plane.b = 0.00000001f;

	pTargetPosition->y = -(plane.a * x + plane.c * z + plane.d) / plane.b;
	pTargetPosition->y += 1.f;

	return true;
}

_uint* CVIBuffer_TerrainTexture::LoadHeightMap(TCHAR* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return nullptr;

	DWORD dwBytes = 0;
	BITMAPFILEHEADER BmpFileHeader;
	ReadFile(hFile, &BmpFileHeader, sizeof(BITMAPFILEHEADER), &dwBytes, 0);

	BITMAPINFOHEADER BmpInfoHeader;
	ReadFile(hFile, &BmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytes, 0);

	_uint iMaxCount = BmpInfoHeader.biWidth * BmpInfoHeader.biHeight;
	_uint* pPixels = new _uint[iMaxCount];
	ReadFile(hFile, pPixels, sizeof(_uint) * iMaxCount, &dwBytes, 0);

	CloseHandle(hFile);

	return pPixels;
}

CVIBuffer_TerrainTexture* CVIBuffer_TerrainTexture::Create(
	LPDIRECT3DDEVICE9 pDevice, 
	_uint iVertexCountX, 
	_uint iVertexCountZ,
	float fVertexInterval/* = 1.f*/)
{
	CVIBuffer_TerrainTexture* pInstance = new CVIBuffer_TerrainTexture(pDevice, iVertexCountX, iVertexCountZ, fVertexInterval);
	if (FAILED(pInstance->ReadyComponentPrototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CVIBuffer_TerrainTexture");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainTexture::Clone(void * pArg)
{
	CVIBuffer_TerrainTexture* pClone = new CVIBuffer_TerrainTexture(*this);
	if (FAILED(pClone->ReadyComponent(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CVIBuffer_TerrainTexture");
		SafeRelease(pClone);
	}

	return pClone;
}

void CVIBuffer_TerrainTexture::Free()
{
	CVIBuffer::Free();
}
