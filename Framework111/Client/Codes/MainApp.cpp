#include "stdafx.h"
#include "MainApp.h"
#include "Logo.h"

CMainApp::CMainApp()
	: m_pManagement(CManagement::Get_Instance())
{
	SafeAddRef(m_pManagement);
}

HRESULT CMainApp::ReadyMainApp()
{
	if (FAILED(m_pManagement->ReadyEngine(g_hWnd, WINCX, WINCY, 
		EDisplayMode::Window)))
	{
		PRINT_LOG(L"Error", L"Failed To ReadyEngine");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->SetUpCurrentScene((_int)ESceneID::Logo,
		CLogo::Create(m_pManagement->GetDevice()))))
	{
		PRINT_LOG(L"Error", L"Failed To SetUpCurrentScene");
		return E_FAIL;
	}

	return S_OK;
}

int CMainApp::UpdateMainApp()
{
	m_pManagement->UpdateEngine();
//	m_pManagement->RenderEngine();

	return 0;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;
	if (FAILED(pInstance->ReadyMainApp()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CMainApp");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	SafeRelease(m_pManagement);
	CManagement::ReleaseEngine();
}
