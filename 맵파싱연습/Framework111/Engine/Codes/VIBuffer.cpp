#include "..\Headers\VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & other)
	: CComponent(other)
	, m_pVB(other.m_pVB)
	, m_pIB(other.m_pIB)
	, m_iVertexSize(other.m_iVertexSize)
	, m_iVertexCount(other.m_iVertexCount)
	, m_iTriCount(other.m_iTriCount)
	, m_iFVF(other.m_iFVF)
	, m_iIndexSize(other.m_iIndexSize)
	, m_IndexFormat(other.m_IndexFormat)
	, m_pVertices(other.m_pVertices)
	, m_IsClone(true)
{
}

HRESULT CVIBuffer::ReadyComponentPrototype()
{
	/* 버텍스 버퍼 생성 */
	if (FAILED(m_pDevice->CreateVertexBuffer(
		m_iVertexSize * m_iVertexCount, /* 버텍스버퍼가 관리할 배열의 총 사이즈 */
		0, /* 0이면 정적버퍼 */
		m_iFVF, /* FVF */
		D3DPOOL_MANAGED, /* 메모리 보관 방식 */
		&m_pVB, /* 할당된 버텍스버퍼의 주소를 반환 */
		nullptr)))
	{
		return E_FAIL;
	}

	/* 인덱스버퍼 생성 */
	if (FAILED(m_pDevice->CreateIndexBuffer(
		m_iIndexSize * m_iTriCount, /* 인덱스버퍼가 관리할 배열의 총 사이즈 */
		0, /* 0이면 정적버퍼 */
		m_IndexFormat, /* 인덱스 포맷 */
		D3DPOOL_MANAGED, /* 메모리 보관 방식 */
		&m_pIB,
		nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::ReadyComponent(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render_VIBuffer()
{
	/* 장치에게 버텍스버퍼 전달 */
	if (FAILED(m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexSize)))
		return E_FAIL;

	/* 장치에게 FVF 전달 */
	if (FAILED(m_pDevice->SetFVF(m_iFVF)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Free()
{
	/* 자식의 리소스 해제 */
	if (false == m_IsClone)
	{
		SafeDeleteArray(m_pVertices);
	}

	
	SafeRelease(m_pVB);
	SafeRelease(m_pIB);

	/* 부모의 free */
	CComponent::Free();
}
