#include "..\Headers\Management.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pGraphic_Dev(CGraphic_Device::Get_Instance())
	, m_pSceneManager(CSceneManager::Get_Instance())
{
	SafeAddRef(m_pGraphic_Dev);
	SafeAddRef(m_pSceneManager);
}

HRESULT CManagement::ReadyEngine(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eDisplaymode)
{
	if (FAILED(m_pGraphic_Dev->Ready_Graphic_Device(hWnd, iWinCX, iWinCY, eDisplaymode)))
		return E_FAIL;

	return S_OK;
}

_uint CManagement::UpdateEngine()
{
	if (nullptr == m_pSceneManager)
		return 0;

	m_iUpdateEvent = m_pSceneManager->UpdateScene();
	if (CHANGE_SCNENE == m_iUpdateEvent)
		return m_iUpdateEvent;

	m_iUpdateEvent = m_pSceneManager->LateUpdateScene();
	if (CHANGE_SCNENE == m_iUpdateEvent)
		return m_iUpdateEvent;

	return _uint();
}

HRESULT CManagement::RenderEngine()
{
	if (CHANGE_SCNENE == m_iUpdateEvent)
		return S_OK;

	m_pGraphic_Dev->Render_Begin();

	m_pGraphic_Dev->Render_End();

	return S_OK;
}

LPDIRECT3DDEVICE9 CManagement::GetDevice()
{
	if (nullptr == m_pGraphic_Dev)
		return nullptr;

	return m_pGraphic_Dev->Get_Device();
}

HRESULT CManagement::SetUpCurrentScene(_int iSceneID, CScene * pCurrentScene)
{
	if (nullptr == m_pSceneManager)
		return E_FAIL;

	return m_pSceneManager->SetUpCurrentScene(iSceneID, pCurrentScene);
}

void CManagement::Free()
{
	SafeRelease(m_pSceneManager);
	SafeRelease(m_pGraphic_Dev);
}

void CManagement::ReleaseEngine()
{
	if(CManagement::Destroy_Instance())
		PRINT_LOG(L"Warning", L"Failed To Release CManagement");

	if (CSceneManager::Destroy_Instance())
		PRINT_LOG(L"Warning", L"Failed To Release CSceneManager");

	if (CGraphic_Device::Destroy_Instance())
		PRINT_LOG(L"Warning", L"Failed To Release CGraphic_Device");
}
