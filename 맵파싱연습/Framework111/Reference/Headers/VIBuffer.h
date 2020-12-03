#pragma once
#ifndef __VIBUFFER_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer(const CVIBuffer& other);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT ReadyComponentPrototype() = 0;
	virtual HRESULT ReadyComponent(void* pArg = nullptr) = 0;
	virtual HRESULT Render_VIBuffer() = 0;

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	/* 
	���ؽ����� �İ�ü: ���� ���ؽ����� �迭 �������� �����ϰ� �����ϴ� �İ�ü.
	*/
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr; 
	_uint m_iVertexSize = 0;
	_uint m_iVertexCount = 0;
	_uint m_iTriCount = 0;
	_uint m_iFVF = 0; /* Flexible Vertex Format(������ ���� ����) */

	/*
	�ε������� �İ�ü: ���� �ε������� �迭 �������� �����ϰ� �����ϴ� �İ�ü.
	*�ε���: �ϳ��� �������� �����ϴ� �������� �̿����踦 ǥ���ϴ� ����.
	*/
	LPDIRECT3DINDEXBUFFER9 m_pIB = nullptr;
	_uint		m_iIndexSize = 0;
	D3DFORMAT	m_IndexFormat;

	void*	m_pVertices = nullptr;
	_bool	m_IsClone = false;


};
END

#define __VIBUFFER_H__
#endif

