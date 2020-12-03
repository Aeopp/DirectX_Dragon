#include "VIBuffer_TerrainColor.h"

USING(Engine)

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(
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

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(const CVIBuffer_TerrainColor & other)
	: CVIBuffer(other)
	, m_iVertexCountX(other.m_iVertexCountX)
	, m_iVertexCountZ(other.m_iVertexCountZ)
	, m_fVertexInterval(other.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainColor::ReadyComponentPrototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iFVF = FVF_VTX_COLOR;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::ReadyComponentPrototype()))
		return E_FAIL;

	VTX_COLOR* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_uint iIndex = 0;

	for (_uint z = 0; z < m_iVertexCountZ; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX; ++x)
		{
			iIndex = z * m_iVertexCountX + x;

			if (m_iVertexCount <= iIndex)
				continue;

			pVertex[iIndex].vPosition = D3DXVECTOR3(x * m_fVertexInterval, 0.f, z * m_fVertexInterval);
			pVertex[iIndex].iColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	m_pVertices = new VTX_COLOR[m_iVertexCount];
	memcpy(m_pVertices, pVertex, sizeof(VTX_COLOR) * m_iVertexCount);

	m_pVB->Unlock();

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

HRESULT CVIBuffer_TerrainColor::ReadyComponent(void * pArg)
{
	if (FAILED(CVIBuffer::ReadyComponent(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_TerrainColor::Render_VIBuffer()
{
	if (FAILED(CVIBuffer::Render_VIBuffer()))
		return E_FAIL;

	/* 장치에게 인덱스버퍼 셋 */
	if (FAILED(m_pDevice->SetIndices(m_pIB)))
		return E_FAIL;

	/* 인덱스를 이용해서 도형을 그리는 함수 */
	return m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount);
}

CVIBuffer_TerrainColor* CVIBuffer_TerrainColor::Create(
	LPDIRECT3DDEVICE9 pDevice, 
	_uint iVertexCountX, 
	_uint iVertexCountZ,
	float fVertexInterval/* = 1.f*/)
{
	CVIBuffer_TerrainColor* pInstance = new CVIBuffer_TerrainColor(pDevice, iVertexCountX, iVertexCountZ, fVertexInterval);
	if (FAILED(pInstance->ReadyComponentPrototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CVIBuffer_TerrainColor");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainColor::Clone(void * pArg)
{
	CVIBuffer_TerrainColor* pClone = new CVIBuffer_TerrainColor(*this);
	if (FAILED(pClone->ReadyComponent(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CVIBuffer_TerrainColor");
		SafeRelease(pClone);
	}

	return pClone;
}

void CVIBuffer_TerrainColor::Free()
{
	CVIBuffer::Free();
}
