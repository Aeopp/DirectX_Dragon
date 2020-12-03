#pragma once
#ifndef __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pDevice);
	explicit CTransform(const CTransform& other);
	virtual ~CTransform() = default;

public:
	const TRANSFORM_DESC& GetTransformDesc() const;

public:
	void SetPosition(const _vector& vPosition);

public:
	// CComponent��(��) ���� ��ӵ�
	virtual HRESULT ReadyComponentPrototype() override;
	virtual HRESULT ReadyComponent(void * pArg = nullptr) override;

public:
	HRESULT UpdateTransform();
	void GoStraight(float fDeltaTime);
	void GoSide(float fDeltaTime);
	void AddRotationX(float fDelaTime);
	void AddRotationY(float fDelaTime);
	void AddRotationZ(float fDelaTime);

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	TRANSFORM_DESC	m_TransformDesc;	
};
END

#define __TRANSFORM_H__
#endif
