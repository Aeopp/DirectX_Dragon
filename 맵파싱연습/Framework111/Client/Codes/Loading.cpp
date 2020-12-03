#include "stdafx.h"
#include "..\Headers\Loading.h"
#include "Terrain.h"
#include "MainCamera.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pDevice, ESceneID eNextSceneID)
	: m_pDevice(pDevice)
	, m_eNextSceneID(eNextSceneID)
{
	SafeAddRef(m_pDevice);
}

_bool CLoading::IsFinished() const
{
	return m_IsFinished;
}

HRESULT CLoading::ReadyLoading()
{
	m_hThread = (HANDLE)_beginthreadex(0, 0, ThreadMain, this, 0, nullptr);
	if (nullptr == m_hThread)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSec);

	return S_OK;
}

HRESULT CLoading::ReadyStageResources()
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	/* For.GameObject */
#pragma region GameObject_Terrain
	if (FAILED(pManagement->AddGameObjectPrototype(
		(_int)ESceneID::Stage,
		L"GameObject_Terrain",
		CTerrain::Create(m_pDevice))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject_Camera
	if (FAILED(pManagement->AddGameObjectPrototype(
		(_int)ESceneID::Stage,
		L"GameObject_Camera",
		CMainCamera::Create(m_pDevice))))
		return E_FAIL;
#pragma endregion

	/* For.Component */
#pragma region Component_VIBuffer_TerrainTexture
	if (FAILED(pManagement->AddComponentPrototype(
		(_int)ESceneID::Stage,
		L"Component_VIBuffer_TerrainTexture",
		CVIBuffer_TerrainTexture::Create(m_pDevice, TERRAIN_VTX_COUNTX, TERRAIN_VTX_COUNTZ))))
		return E_FAIL;
#pragma endregion

#pragma region Component_Texture_Terrain
	if (FAILED(pManagement->AddComponentPrototype(
		(_int)ESceneID::Stage,
		L"Component_Texture_Terrain",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../Resources/Terrain/Terrain%d.png"))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pDevice, ESceneID eNextSceneID)
{
	CLoading* pInstance = new CLoading(pDevice, eNextSceneID);
	if (FAILED(pInstance->ReadyLoading()))
	{
		PRINT_LOG(L"Warning", L"Failed To Create Loading");
		SafeRelease(pInstance);		
	}

	return pInstance;
}

void CLoading::Free()
{
	SafeRelease(m_pDevice);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSec);
}

unsigned CLoading::ThreadMain(void * pParam)
{
	CLoading* pLoading = (CLoading*)pParam;
	if (nullptr == pLoading)
		return 0;

	EnterCriticalSection(&pLoading->m_CriticalSec);

	HRESULT hr = 0;

	switch (pLoading->m_eNextSceneID)
	{
	case ESceneID::Menu:
		//hr = pLoading->ReadyStageResources();
		break;
	case ESceneID::Stage:
		hr = pLoading->ReadyStageResources();
		break;
	}

	LeaveCriticalSection(&pLoading->m_CriticalSec);

	if (FAILED(hr))
		return 0;		

	pLoading->m_IsFinished = true;

	return 0;
}
