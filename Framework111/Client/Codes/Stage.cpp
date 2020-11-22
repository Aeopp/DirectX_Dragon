#include "stdafx.h"
#include "..\Headers\Stage.h"
#include "..\Headers\d3dUtility.h"
#include <vector>
#include <fstream>
#include <array>

#include "Camera.h"
#include "fps.h"
#include "Terrain.h"

#pragma region Declare

#pragma endregion Declare

#pragma region global_data
constexpr int Width = WINCX;
constexpr int Height = WINCY;
Terrain* TheTerrain = 0;
Camera   TheCamera;
FPSCounter* FPS = 0;

#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto * Device = m_pDevice;

	//
	// Create the terrain.
	//

	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
	TheTerrain = new Terrain(Device, L"coastMountain64.raw", 64, 64, 10, 2.f);
	TheTerrain->genTexture(&lightDirection);



	FPS = new FPSCounter(Device);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

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

	constexpr double timeDelta = 1.f / 5000.f;
	static bool f = false;
	
	if (Device)
	{
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			TheCamera.MoveForward(100.0f, timeDelta);

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			TheCamera.MoveForward(-100.0f , timeDelta);

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TheCamera.Yaw(-1.0f, timeDelta);

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TheCamera.Yaw(1.0f,timeDelta);

		if (::GetAsyncKeyState('N') & 0x8000f)
			TheCamera.MoveRight(-100.0f, timeDelta);

		if (::GetAsyncKeyState('M') & 0x8000f)
			TheCamera.MoveRight(100.0f, timeDelta);

		if (::GetAsyncKeyState('W') & 0x8000f)
			TheCamera.Pitch(1.0f , timeDelta);

		if (::GetAsyncKeyState('S') & 0x8000f)
			TheCamera.Pitch(-1.0f , timeDelta);

		if (::GetAsyncKeyState(VK_SPACE) & 0x8000f)
			f = !f;

		D3DXVECTOR3 pos =TheCamera.GetLocation();
		float height = TheTerrain->getHeight(pos.x, pos.z);
		pos.y = height + 5.0f; // add height because we're standing up
		TheCamera.SetLocation(pos);

		
		D3DXMATRIX V = TheCamera.GetViewMatrix();
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		Device->BeginScene();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);

		if (TheTerrain)
			TheTerrain->draw(&I, f);

		if (FPS)
			FPS->render(0xffffffff, timeDelta);

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

	
}
