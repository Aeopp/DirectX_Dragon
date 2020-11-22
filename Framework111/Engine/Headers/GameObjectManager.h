#pragma once
#ifndef __GAMEOBJECTMANAGER_H__

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameObjectManager final : public CBase
{
	DECLARE_SINGLETON(CGameObjectManager)

private:
	CGameObjectManager();
	virtual ~CGameObjectManager() = default;

public:
	HRESULT ReserveSizePrototypeContainer(_int iSceneCount);
	HRESULT AddGameObjectPrototype(_int iSceneIndex, const wstring& GameObjectTag, CGameObject* pPrototype);
	CGameObject* CloneGameObjectPrototype(_int iSceneIndex, const wstring& GameObjectTag, void* pArg = nullptr);
	HRESULT ClearForScene(_int iSceneIndex);

public:
	_uint UpdateGameObject(float fDeltaTime);
	_uint LateUpdateGameObject(float fDeltaTime);

public:
	virtual void Free() override;

private:
	typedef unordered_map<wstring, CGameObject*> PROTOTYPES;
	PROTOTYPES*	m_pPrototypes = nullptr;

	//typedef unordered_map<wstring, CLayer*

	_int m_iSceneCount = 0;
	
};
END

#define __GAMEOBJECTMANAGER_H__
#endif

