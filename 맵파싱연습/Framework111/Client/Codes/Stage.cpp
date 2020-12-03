#include "stdafx.h"
#include "..\Headers\Stage.h"


CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage::ReadyScene()
{
	CScene::ReadyScene();	

	if (FAILED(AddPlayerLayer(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(AddTerrainLayer(L"Layer_Terrain")))
		return E_FAIL;

	if (FAILED(AddCameraLayer(L"Layer_Camera")))
		return E_FAIL;

	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	if (FAILED(pManagement->AddGameObjectInLayer(
		(_int)ESceneID::Stage, L"Map",
		(_int)ESceneID::Stage, L"Layer_Map")))
		return E_FAIL;

	return S_OK;
}

_uint CStage::UpdateScene()
{
	CScene::UpdateScene();

	return _uint();
}

_uint CStage::LateUpdateScene()
{
	CScene::LateUpdateScene();

	return _uint();
}

HRESULT CStage::AddPlayerLayer(const wstring & LayerTag)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	if (FAILED(pManagement->AddGameObjectInLayer(
		(_int)ESceneID::Static, L"GameObject_Player",
		(_int)ESceneID::Stage, LayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage::AddTerrainLayer(const wstring & LayerTag)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	if (FAILED(pManagement->AddGameObjectInLayer(
		(_int)ESceneID::Stage, L"GameObject_Terrain",
		(_int)ESceneID::Stage, LayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage::AddCameraLayer(const wstring & LayerTag)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CAMERA_DESC CameraDesc;
	CameraDesc.vUp = _vector(0.f, 1.f, 0.f);
	CameraDesc.fFovY = 90.f;
	CameraDesc.fAspect = (float)WINCX / WINCY;
	CameraDesc.fNear = 1.f;
	CameraDesc.fFar = 500.f;

	if (FAILED(pManagement->AddGameObjectInLayer(
		(_int)ESceneID::Stage, L"GameObject_Camera",
		(_int)ESceneID::Stage, LayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

CStage * CStage::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
		return nullptr;

	CStage* pInstance = new CStage(pDevice);
	if (FAILED(pInstance->ReadyScene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CStage");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CStage::Free()
{
	CScene::Free();
}
