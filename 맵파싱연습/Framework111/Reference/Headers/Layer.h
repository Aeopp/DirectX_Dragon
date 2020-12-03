#pragma once
#ifndef __LAYER_H__

#include "Base.h"

BEGIN(Engine)
class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CGameObject* GetGameObject(_uint iIndex = 0);

public:
	HRESULT AddGameObjectInLayer(class CGameObject* pGameObject);
	_uint UpdateGameObject(float fDeltaTime);
	_uint LateUpdateGameObject(float fDeltaTime);

public:
	static CLayer* Create();
	virtual void Free() override;

private:
	typedef list<class CGameObject*>	GAMEOBJECTS;
	GAMEOBJECTS	m_GameObjects;	/* ���� ������Ʈ�� Ŭ�е��� ���� */
};
END

#define __LAYER_H__
#endif