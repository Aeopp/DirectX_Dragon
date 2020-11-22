#include "stdafx.h"
#include "..\Headers\Stage.h"
#include "..\Headers\d3dUtility.h"
#include <vector>
#include <fstream>
#include <array>

#include "Camera.h"

#pragma region Declare

#pragma endregion Declare

#pragma region global_data
constexpr int Width = WINCX;
constexpr int Height = WINCY;
Camera TheCamera;

#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto * Device = m_pDevice;

	d3d::DrawBasicScene(Device, 0.0f);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
}


void CStage::Render()&
{
	auto* Device = m_pDevice;

	constexpr double timeDelta = 1.f / 500.f;
	
	if (Device)
	{
		//
		// Update: Update the camera.
		//

		if (::GetAsyncKeyState('W') & 0x8000f)
			TheCamera.MoveForward(4.0f , timeDelta);

		if (::GetAsyncKeyState('S') & 0x8000f)
			TheCamera.MoveForward(-4.0f, timeDelta);

		if (::GetAsyncKeyState('A') & 0x8000f)
			TheCamera.MoveRight(-4.0f, timeDelta);

		if (::GetAsyncKeyState('D') & 0x8000f)
			TheCamera.MoveRight(4.0f, timeDelta);

		if (::GetAsyncKeyState('R') & 0x8000f)
			TheCamera.MoveUp(4.0f, timeDelta);

		if (::GetAsyncKeyState('F') & 0x8000f)
			TheCamera.MoveUp(-4.0f, timeDelta);

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			TheCamera.Pitch(1.0f, timeDelta);

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			TheCamera.Pitch(-1.0f, timeDelta);

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TheCamera.Yaw(-1.0f, timeDelta);

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TheCamera.Yaw(1.0f, timeDelta);

		if (::GetAsyncKeyState('N') & 0x8000f)
			TheCamera.Roll(1.0f, timeDelta);

		if (::GetAsyncKeyState('M') & 0x8000f)
			TheCamera.Roll(-1.0f, timeDelta);

		// Update the view matrix representing the cameras 
		// new position/orientation.
		
		D3DXMATRIX V = TheCamera.GetViewMatrix();
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		d3d::DrawBasicScene(Device, 1.0f);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
}



CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage::ReadyScene()
{
	CScene::ReadyScene();

	Initialize();
	
	return S_OK;
}

_uint CStage::UpdateScene()
{
	CScene::UpdateScene();

	Render();

	return _uint();
}

_uint CStage::LateUpdateScene()
{
	CScene::LateUpdateScene();

	return _uint();
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

	d3d::DrawBasicScene(0, 0.f);
}
