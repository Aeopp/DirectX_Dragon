#pragma once
#ifndef __GAMEOBJECT_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CGameObject() = default;	

public:
	virtual HRESULT ReadyGameObjectPrototype() = 0;	/* 프로토타입 초기화 */
	virtual HRESULT ReadyGameObject(void* pArg = nullptr) = 0; /* 클론 초기화 */
	virtual _uint UpdateGameObject(float fDeltaTime) = 0;
	virtual _uint LateUpdateGameObject(float fDeltaTime) = 0;
	virtual HRESULT RenderGameObject() = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	LPDIRECT3DDEVICE9	m_pDevice;

	unordered_map<wstring, class CComponent*>	m_Components;
};
END

#define __GAMEOBJECT_H__
#endif