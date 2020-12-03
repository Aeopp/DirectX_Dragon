#include "VIBuffer_TriColor.h"

USING(Engine)

CVIBuffer_TriColor::CVIBuffer_TriColor(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_TriColor::CVIBuffer_TriColor(const CVIBuffer_TriColor & other)
	: CVIBuffer(other)
{
}

HRESULT CVIBuffer_TriColor::ReadyComponentPrototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = 3;
	m_iFVF = FVF_VTX_COLOR;
	m_iTriCount = 1;

	if (FAILED(CVIBuffer::ReadyComponentPrototype()))
		return E_FAIL;

	VTX_COLOR* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = D3DXVECTOR3(0.f, 1.f, 0.f);
	pVertex[0].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertex[1].vPosition = D3DXVECTOR3(1.f, -1.f, 0.f);
	pVertex[1].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertex[2].vPosition = D3DXVECTOR3(-1.f, -1.f, 0.f);
	pVertex[2].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVertices = new VTX_COLOR[m_iVertexCount];
	memcpy(m_pVertices, pVertex, sizeof(VTX_COLOR) * m_iVertexCount);

	m_pVB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TriColor::ReadyComponent(void * pArg)
{
	if (FAILED(CVIBuffer::ReadyComponent(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_TriColor::Render_VIBuffer()
{
	if (FAILED(CVIBuffer::Render_VIBuffer()))
		return E_FAIL;

	/* 도형을 그리는 함수 */
	return m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iTriCount);
}

CVIBuffer_TriColor* CVIBuffer_TriColor::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_TriColor* pInstance = new CVIBuffer_TriColor(pDevice);
	if (FAILED(pInstance->ReadyComponentPrototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CVIBuffer_TriColor");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TriColor::Clone(void * pArg)
{
	CVIBuffer_TriColor* pClone = new CVIBuffer_TriColor(*this);
	if (FAILED(pClone->ReadyComponent(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CVIBuffer_TriColor");
		SafeRelease(pClone);
	}

	return pClone;
}

void CVIBuffer_TriColor::Free()
{
	CVIBuffer::Free();
}
