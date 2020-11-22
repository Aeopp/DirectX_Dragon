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

ID3DXMesh* Mesh = 0;
std::vector<D3DMATERIAL9>       Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);

ID3DXMesh* SphereMesh = 0;
ID3DXMesh* BoxMesh = 0;

bool RenderBoundingSphere = false;
#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto * Device = m_pDevice;

	HRESULT hr = 0;

	//
	// Load the XFile data.
	//
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&Mesh);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		
	}

	//
	// Extract the materials, load textures.
	//

	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			Mtrls.push_back(mtrls[i].MatD3D);

			// check if the ith material has an associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFileA(
					Device,
					mtrls[i].pTextureFilename,
					&tex);

				// save the loaded texture
				Textures.push_back(tex);
			}
			else
			{
				// no texture for the ith subset
				Textures.push_back(0);
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer); // done w/ buffer

	//
	// Optimize the mesh.
	//

	hr = Mesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

	if (FAILED(hr))
	{
		::MessageBox(0, L"OptimizeInplace() - FAILED", 0, 0);
	}

	//
	// Compute Bounding Sphere and Bounding Box.
	//

	std::pair<d3d::BoundingSphere ,bool > boundingSphere;
	std::pair<d3d::BoundingBox    ,bool> boundingBox;

	boundingSphere=d3d::BoundingSphere::ComputeBoundingSphere(Mesh);
	boundingBox = d3d::BoundingBox::ComputeBoundingBox(Mesh);
	
	
	D3DXCreateSphere(
		Device,
		boundingSphere.first._radius,
		20,
		20,
		&SphereMesh,
		0);

	D3DXCreateBox(
		Device,
		boundingBox.first._max.x - boundingBox.first._min.x,
		boundingBox.first._max.y - boundingBox.first._min.y,
		boundingBox.first._max.z - boundingBox.first._min.z,
		&BoxMesh,
		0);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Set Lights.
	//

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(4.0f, 12.0f, -20.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
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
		if (Device)
		{
			//
			// Update: Rotate the mesh.
			//

			static float y = 0.0f;
			D3DXMATRIX yRot;
			D3DXMatrixRotationY(&yRot, y);
			y += timeDelta;

			if (y >= 6.28f)
				y = 0.0f;

			D3DXMATRIX World = yRot;

			Device->SetTransform(D3DTS_WORLD, &World);

			//
			// Render
			//

			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			Device->BeginScene();

			// draw the mesh
			for (int i = 0; i < Mtrls.size(); i++)
			{
				Device->SetMaterial(&Mtrls[i]);
				Device->SetTexture(0, Textures[i]);
				Mesh->DrawSubset(i);
			}

			//
			// Draw bounding volume in blue and at 10% opacity
			D3DMATERIAL9 blue = d3d::BLUE_MTRL;
			blue.Diffuse.a = 0.10f; // 10% opacity

			Device->SetMaterial(&blue);
			Device->SetTexture(0, 0); // disable texture

			Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			if (RenderBoundingSphere)
				SphereMesh->DrawSubset(0);
			else
				BoxMesh->DrawSubset(0);

			Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			Device->EndScene();
			Device->Present(0, 0, 0, 0);
		}
		
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
