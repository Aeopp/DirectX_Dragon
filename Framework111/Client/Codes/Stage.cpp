#include "stdafx.h"
#include "..\Headers\Stage.h"
#include "..\Headers\d3dUtility.h"
#include <vector>
#include <fstream>
#include <array>
#include "pSystem.h"
#include "Camera.h"
#include "fps.h"
#include "Terrain.h"
#include <ctime>
#include <fstream>
#include <ostream>
#include <sstream>

#pragma region Declare

#pragma endregion Declare


#pragma region global_data
constexpr int Width = WINCX;
constexpr int Height = WINCY;

IDirect3DVertexShader9* ToonShader = nullptr;
ID3DXConstantTable* ToonConstTable = nullptr;

ID3DXMesh* Meshes[4] = { 0, 0, 0, 0 };
D3DXMATRIX WorldMatrices[4];
D3DXVECTOR4 MeshColors[4];

D3DXMATRIX ProjMatrix;

IDirect3DTexture9* ShadeTex = 0;

D3DXHANDLE WorldViewHandle = 0;
D3DXHANDLE WorldViewProjHandle = 0;
D3DXHANDLE ColorHandle = 0;
D3DXHANDLE LightDirHandle = 0;
#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto* Device = m_pDevice;
	
	HRESULT hr = 0;

	D3DXCreateTeapot(Device, &Meshes[0], 0);
	D3DXCreateSphere(Device, 1.f, 20, 20, &Meshes[1], 0);
	D3DXCreateTorus(Device, 0.5f, 1.f, 20, 20, &Meshes[2], 0);
	D3DXCreateCylinder
	(Device, 0.5f, 0.5f, 2.f, 20, 20, &Meshes[3], 0);

	D3DXMatrixTranslation(&WorldMatrices[0], 0.0f, 2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[1], 0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[2], -3.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[3], 3.0f, 0.0f, 0.0f);

	MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

	ID3DXBuffer* shader = 0;
	ID3DXBuffer* errorBuffer = 0;
	
	hr = D3DXCompileShaderFromFile(
		L"toon.hlsl",
		0, 0,
		"main",
		"vs_1_1",
		D3DXSHADER_DEBUG
		,
		&shader,
		&errorBuffer,
		&ToonConstTable);

	if (errorBuffer)
	{
		::MessageBox(0, (wchar_t*)errorBuffer->GetBufferPointer()
			, 0, 0);
		d3d::Release(errorBuffer);
	};

	if (FAILED(hr))
	{
		::MessageBox(0, __FUNCTIONW__, 0, 0);
	}
	hr = Device->CreateVertexShader(
		(DWORD*)shader->GetBufferPointer(),
		&ToonShader);

	if (FAILED(hr))
	{
		::MessageBox(0, __FUNCTIONW__, 0, 0);

	}

	d3d::Release(shader);

	D3DXCreateTextureFromFile(Device,
		L"toonshade.bmp", &ShadeTex);
	
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	WorldViewHandle = ToonConstTable->GetConstantByName
	(0, "WorldViewMatrix");
	WorldViewProjHandle = ToonConstTable->GetConstantByName
	(0, "WorldViewProjMatrix");
	ColorHandle = ToonConstTable->GetConstantByName
	(0, "Color");
	LightDirHandle = ToonConstTable->GetConstantByName
	(0, "LightDirection");

	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.0f);

	ToonConstTable->SetVector(
		Device,
		LightDirHandle,
		&directionToLight);

	ToonConstTable->SetDefaults(Device);

	D3DXMatrixPerspectiveFovLH(
		&ProjMatrix, D3DX_PI * 0.25f,
		(float)Width / Height, 1.f, 1000.f);



#pragma region TerrainVertexCalc

#pragma endregion TerrainVertexCalc
}


void CStage::Render()&
{
	auto* Device = m_pDevice;


	constexpr float timeDelta = 1.f / 200.f;

	if (Device)
	{
		static float angle = (3.f * D3DX_PI) / 2.f;
		static float height = 5.0f;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			angle += 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			height += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			height -= 5.0f * timeDelta;


		D3DXVECTOR3 position(cosf(angle) * 7.0f,
			height, sinf(angle) * 7.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);

		Device->Clear(0, 0, D3DCLEAR_TARGET |
			D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetVertexShader(ToonShader);
		Device->SetTexture(0, ShadeTex);

		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProj;

		for (int i = 0; i < 4; i++)
		{
			WorldView = WorldMatrices[i] * view;
			WorldViewProj = WorldMatrices[i] * view * ProjMatrix;

			ToonConstTable->SetMatrix(
				Device,
				WorldViewHandle,
				&WorldView);

			ToonConstTable->SetMatrix(
				Device,
				WorldViewProjHandle,
				&WorldViewProj);

			ToonConstTable->SetVector(
				Device,
				ColorHandle,
				&MeshColors[i]);

			Meshes[i]->DrawSubset(0);
		}
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

	for (int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(Meshes[i]);
	}
	d3d::Release<IDirect3DTexture9*>(ShadeTex);
	d3d::Release<IDirect3DVertexShader9*>(ToonShader);
	d3d::Release<ID3DXConstantTable*>(ToonConstTable);


	d3d::DrawBasicScene(0, 1.f);
}
