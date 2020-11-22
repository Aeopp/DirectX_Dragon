#include "stdafx.h"
#include "..\Headers\Stage.h"
#include "..\Headers\d3dUtility.h"
#include <vector>
#include <fstream>
#include <array>

#pragma region Declare

#pragma endregion Declare

#pragma region global_data
constexpr int Width = WINCX;
constexpr int Height = WINCY;
ID3DXMesh* SourceMesh = nullptr;
ID3DXPMesh* PMesh = nullptr;

std::vector<D3DMATERIAL9> Mtrls{ 0 };
std::vector<IDirect3DTexture9*> Textures{ 0 };
#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto * Device = m_pDevice;

	HRESULT hr = 0;

	ID3DXBuffer* adjBuffer = nullptr;
	ID3DXBuffer* mtrlBuffer = nullptr;
	DWORD numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&SourceMesh);

	if(FAILED(hr))
	{
		::MessageBox(g_hWnd, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
	}

	if(mtrlBuffer!=nullptr && numMtrls!=0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i=0;i<numMtrls;++i)
		{
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			Mtrls.push_back(mtrls[i].MatD3D);

			if(mtrls[i].pTextureFilename!=nullptr)
			{
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFileA(
					Device,
					mtrls[i].pTextureFilename,
					&tex);

				Textures.push_back(tex);
			}
			else
			{
				Textures.push_back(nullptr);
			}
		}

		d3d::Release(mtrlBuffer);
	}

	hr = SourceMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		(DWORD*)adjBuffer->GetBufferPointer(), // new adjacency info
		0, 0);

	
	if(FAILED(hr))
	{
		::MessageBox(0, L"OptInplace FAIL", 0, 0);
	}

	hr = D3DXGeneratePMesh(SourceMesh,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 1, D3DXMESHSIMP_FACE, &PMesh);


	d3d::Release(SourceMesh);
	d3d::Release(adjBuffer);

	if(FAILED(hr))
	{
		::MessageBox(0, L"D3DXGeneratePMesh() - FAILED", 0, 0);
	}

	DWORD maxFaces = PMesh->GetMaxFaces();
	PMesh->SetNumFaces(maxFaces);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	D3DCAPS9 _Cap;
	Device->GetDeviceCaps(&_Cap);
	Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, _Cap.MaxAnisotropy);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	_vector dir{ 1,-1,1 };
	D3DXCOLOR col{ 1,1,1,1 };
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	_vector pos(-8.0f, 4.0f, -12.0f);
	_vector target{ 0,0,0 };
	_vector up{ 0 , 1 ,   0 };

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V,
		&pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj, D3DX_PI * 0.5f,
		(float)WINCX / (float)WINCY,
		1.f, 1000.f);

	Device->SetTransform(D3DTS_PROJECTION, &proj);
}

static void RenderScene(IDirect3DDevice9* Device)
{
	
	
};
static void RenderShadow(IDirect3DDevice9* Device)
{
	
};

void CStage::Render()&
{
	auto* Device = m_pDevice;

	constexpr double timeDelta = 1.f / 1000.f;
	static float radius = 20.f;
	
	if(Device )
	{
		int numFaces = PMesh->GetNumFaces();
		
		if(::GetAsyncKeyState('A')& 0x8000f)
		{
			PMesh->SetNumFaces(numFaces + 1);
			if (PMesh->GetNumFaces() == numFaces)
				PMesh->SetNumFaces(numFaces + 2);
		}

		if(::GetAsyncKeyState('S') & 0x8000f)
		{
			PMesh->SetNumFaces(numFaces - 1);
		}

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);
		Device->BeginScene();

		for(int i=0;i<Mtrls.size();++i)
		{
			Device->SetMaterial(&Mtrls[i]);
			Device->SetTexture(0, Textures[i]);
			PMesh->DrawSubset(i);

			Device->SetMaterial(&d3d::YELLOW_MTRL);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			PMesh->DrawSubset(i);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

		Device->EndScene();
		Device->Present(0, 0, g_hWnd, 0);
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

	d3d::Release(SourceMesh);

	for(int i=0;i<Textures.size();++i)
	{
		d3d::Release(Textures[i]); 
	}
}
