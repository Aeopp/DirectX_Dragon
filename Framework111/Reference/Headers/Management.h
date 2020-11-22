#pragma once
#ifndef __MANAGEMENT_H__

#include "Base.h"
#include "Graphic_Device.h"
#include "SceneManager.h"

BEGIN(Engine)
class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	CManagement();
	virtual ~CManagement() = default;

public:
	/* For.General */
	HRESULT ReadyEngine(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eDisplaymode);
	_uint UpdateEngine();
	HRESULT RenderEngine();

public:
	/* For.GraphicDev */
	LPDIRECT3DDEVICE9 GetDevice();

public:
	/* For.SceneManager */
	HRESULT SetUpCurrentScene(_int iSceneID, CScene* pCurrentScene);

public:
	virtual void Free() override;
	static void ReleaseEngine();

private:
	CGraphic_Device*	m_pGraphic_Dev = nullptr;
	CSceneManager*		m_pSceneManager = nullptr;

	_uint m_iUpdateEvent = 0;
};
END

#define __MANAGEMENT_H__
#endif