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
struct Vertex
{
	_vector Location;
	_vector Normal;
	D3DXVECTOR2 TexCoord;
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

struct TerrainVertex
{
	_vector Location;
	D3DCOLOR _Color;
//	_vector Normal;
	static const DWORD FVF;
};

const DWORD TerrainVertex::FVF =
D3DFVF_XYZ |
D3DFVF_DIFFUSE;
//D3DFVF_NORMAL;
#pragma endregion Declare


#pragma region global_data
constexpr int Width = WINCX;
constexpr int Height = WINCY;
D3DLIGHT9 _Light;
Camera   TheCamera;
_vector CameraPos{ 0,0,-10 };
IDirect3DVertexBuffer9* _GunVtxBuffer;

_int PT_Count = 0;
D3DMATERIAL9 _Mtrl = d3d::WHITE_MTRL;
IDirect3DVertexBuffer9* _TerrainVtxBuffer = nullptr;
IDirect3DIndexBuffer9* _TerrainIdxBuffer = nullptr;
_vector SpotLightLocation  { 0, 10000,0 };
D3DLIGHT9 SpotLight;
#pragma endregion global_data

void CStage::Initialize() & noexcept
{
	auto* Device = m_pDevice;
	// TODO :: 매개변수
	std::wstring _ObjFileName = L"Gun.obj";
	const std::wstring V = L"v";
	const std::wstring Vt = L"vt";
	const std::wstring VN = L"vn";
	const std::wstring F = L"f";
	const wchar_t FDelim = L'\/';
	using _VtxType = Vertex;
	size_t VtxElementCount = 3;
	// TODO :: 또한 파싱한 다음 버텍스 속성 인덱스를 참조해 사용자 정의 버텍스를 만들때에도 순서에 유의 해야한다.
	// END..................
	
	std::wfstream _ObjStream(_ObjFileName);
	if (!_ObjStream.is_open())
		::MessageBox(nullptr, __FUNCTIONW__, 
		L"File Open Fail", 0);

	std::vector<_VtxType> _Vtxs;
	std::vector<_vector> _Locations;
	std::vector<_vector> _Normals;
	std::vector<D3DXVECTOR2> _TextureCoords;
	
	std::wstringstream wss;
	std::wstring Line;
	
	while(_ObjStream)
	{
		// Line Read.
		std::getline(_ObjStream, Line);
		wss .str(Line) ;
		std::wstring ToKen;
		wss >> ToKen;
		
		if(ToKen == V)
		{
			_vector VertexLocation{ 0,0,0 };
			 wss >> VertexLocation.x;
			 wss >> VertexLocation.y;
			 wss >> VertexLocation.z;
			 _Locations.push_back(std::move(VertexLocation));
		}
		else if (ToKen==Vt)
		{
			D3DXVECTOR2 VertexTexCoord{ 0,0 };
			wss >> VertexTexCoord.x;
			wss >> VertexTexCoord.y;
			_TextureCoords.push_back(std::move(VertexTexCoord));
		}
		else if(ToKen==VN)
		{
			_vector VertexNormal{ 0,0,0 };
			wss >> VertexNormal.x;
			wss >> VertexNormal.y;
			wss >> VertexNormal.z;
			_Normals.push_back(std::move(VertexNormal));
		}
		else if(ToKen==F)
		{
			std::wstring ReplaceLine;
			std::getline(wss, ReplaceLine);
			//   FDelim (/)  <- 를 구분자로 하여서 파싱하기 편한 형태로 문자열을 변형
			std::replace_copy(std::begin(ReplaceLine), std::end(ReplaceLine),
				std::begin(ReplaceLine), FDelim, L' ');
			wss.str(ReplaceLine);
			wss.clear();
			for(size_t i=0;i< 3;++i)
			{
				// 이쪽 코드는 버텍스의 속성에 종속
				_int LocationIdx, NormalIdx, TexCoordIdx;
				wss >> LocationIdx;
				wss >> TexCoordIdx;
				wss >> NormalIdx;
				
				_Vtxs.emplace_back
				(
					_VtxType
					{
						_Locations[LocationIdx-1],
						_Normals[NormalIdx - 1],
						_TextureCoords[TexCoordIdx-1]
					}
				);
			}
			ReplaceLine.clear();
		}
		wss.str(std::wstring{});
		Line.clear();
		wss.clear();
	}

	if(FAILED  ( 
	m_pDevice->CreateVertexBuffer(sizeof(Vertex) * 
		_Vtxs.size() ,
		0, Vertex::FVF, D3DPOOL_MANAGED,
		&_GunVtxBuffer, 0)))
	{
		MessageBox(nullptr, __FUNCTIONW__, nullptr, 0);
	}

	PT_Count = _Vtxs.size() / 3;
	Vertex* _VtxBuffer;
	
	_GunVtxBuffer->Lock(0, 0, (void**)&_VtxBuffer, 0);	
	memcpy(_VtxBuffer, _Vtxs.data(),sizeof(_VtxType)* _Vtxs.size());
	_GunVtxBuffer->Unlock();

	_matrix proj;
	
	D3DXMatrixPerspectiveFovLH(
		&proj, 3.141592f / 2.f, (float)WINCX / WINCY,
		1.f, 1000.f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);



#pragma region TerrainVertexCalc
	constexpr float SizeScale = 1.f;
	constexpr float Width = 1000.f * SizeScale;
	constexpr float Height = 1000.f * SizeScale;
	constexpr float _DiffScale = 1.f;
	constexpr float XAxisDiff  = 1 *_DiffScale;
	constexpr float ZAxisDiff = 1 * _DiffScale;
	constexpr _int XNum = (Width) / XAxisDiff;
	constexpr _int ZNum = (Height) / ZAxisDiff;
	constexpr DWORD VtxNumber = XNum * ZNum;
	constexpr DWORD IdxCount = (XNum - 1) * (ZNum - 1) * 6;
	const _vector TerrainNormal = { 0,-1,0 };
	const _vector Correction = { Width / 2.f,0,Height / 2.f };
	D3DXCOLOR XLeftColor = D3DXCOLOR{ 124.f/255.f,181.f/255.f,255/255.f,1.f };
	D3DXCOLOR XRightColor = D3DXCOLOR{ 255.f/255.f,  230 / 255.f,91.f / 255.f,0.25f };
	D3DXCOLOR ZNearColor = D3DXCOLOR{ 112.f/255.f,255.f/255.f,121/255.f,1.f };
	D3DXCOLOR ZFarColor = D3DXCOLOR{ 248.f / 255.f,63.f / 255.f, 255.f / 255.f,0.25f};
	// TODO :: 여기까지가 매개변수 세팅 아래서부턴 알고리즘 루프

	Device->CreateVertexBuffer
	(
		VtxNumber * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&_TerrainVtxBuffer,
		0);
	
	_vector Start = { 0,0,0 };
	_vector End = { Width ,0,Height };
	
	// TODO :: 1. 좌표 계산 이후에 원점으로 이동시키자.
	
	TerrainVertex* _TerrainVtxs = nullptr;

	// Col is XAxis  Row is ZAxis
	size_t _TerrainVtxBufIdx = 0;

	// 버텍스 락
	_TerrainVtxBuffer->Lock(0, 0, (void**)&_TerrainVtxs, 0);
	for(_int Row =0; Row <ZNum;++Row)
	{
		for (_int Col = 0; Col < XNum; ++Col)
		{
			_vector Diff = { XAxisDiff * Col, 0 , ZAxisDiff * Row };
			_vector _Vtx = Start + Diff;
			DWORD Dia = Col + Row;
			if (Dia > (ZNum * XNum) / 2)
			{
				Dia = (ZNum * XNum) -  Dia;
			}
			float lerp_x = (float)XNum / Dia;
			float lerp_z = (float)ZNum/Dia;
			D3DXCOLOR lerp_x_color, lerp_z_color, CurColor;
			D3DXColorLerp(&lerp_x_color, &XLeftColor, &XRightColor, lerp_x);
			D3DXColorLerp(&lerp_z_color, &ZNearColor, &ZFarColor, lerp_z);
			// X -> Z
			_vector ToZ = _vector{ 0,0,(float)Dia } - _vector{ (float)Dia,0,0 };
			float length = D3DXVec3Length(&ToZ);
			_vector Cur = _vector{ (float)Col,0,(float)Row };
			float s= length / D3DXVec3Length(&_vector{ Cur - _vector{ (float)Dia,0,0 } });
			D3DXColorLerp(&CurColor, &XLeftColor,& ZNearColor, s);
			if(Col ==XNum/2  &&Row== ZNum/2)
			{
				int Debug = 0;
			}
			
			_Vtx -= Correction;
			_TerrainVtxs[_TerrainVtxBufIdx].Location = _Vtx;			
		//	_TerrainVtxs[_TerrainVtxBufIdx].Normal = TerrainNormal;
			 D3DCOLOR _color = D3DCOLOR_ARGB( DWORD( CurColor.a*255.f), DWORD(CurColor.r * 255.f), DWORD(CurColor.g * 255.f), DWORD(CurColor.b * 255.f));
			_TerrainVtxs[_TerrainVtxBufIdx]._Color = _color;
			
			// 색깔 넣기
			_TerrainVtxBufIdx++;
		}
	}
	_TerrainVtxBuffer->Unlock();
	
	Device->CreateIndexBuffer(
		IdxCount * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&_TerrainIdxBuffer,
		0);
	DWORD* _IdxBuf;

	_TerrainIdxBuffer->Lock(0, 0,(void**)&_IdxBuf, 0);
	size_t _IdxBufIdx = 0;
	for(int Row=0;Row< ZNum-1;++Row)
	{
		for(int Col=0;Col<XNum-1;++Col)
		{
			const _int RT = (Col + 1) + (Row + 1) * ZNum;
			const _int RB = (Col + 1) + (Row)*ZNum;
			const _int LT = (Col)+(Row + 1) * ZNum;
			const _int LB = (Col)+(Row)*ZNum;

			//First
			_IdxBuf[_IdxBufIdx++] = LB;
			_IdxBuf[_IdxBufIdx++] = LT;
			_IdxBuf[_IdxBufIdx++] = RT;
			// Second
			_IdxBuf[_IdxBufIdx++] = RT;
			_IdxBuf[_IdxBufIdx++] = RB;
			_IdxBuf[_IdxBufIdx++] = LB;
		}
	}
	// 인덱스 언락
#pragma endregion TerrainVertexCalc
}


void CStage::Render()&
{
	float Speed = 1;
	auto* Device = m_pDevice;
	if (::GetAsyncKeyState('A') & 0x8000)
		TheCamera.MoveRight(Speed, 1.f);
	if (::GetAsyncKeyState('D') & 0x8000)
		TheCamera.MoveRight(-Speed, 1.f);
	if (::GetAsyncKeyState('Q') & 0x8000)
		TheCamera.MoveUp(Speed, 1.f);
	if (::GetAsyncKeyState('E') & 0x8000)
		TheCamera.MoveUp(-Speed, 1.f);
	if (::GetAsyncKeyState('W') & 0x8000)
		TheCamera.MoveForward(Speed, 1.f);
	if (::GetAsyncKeyState('S') & 0x8000)
		TheCamera.MoveForward(-Speed, 1.f);

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000)
		TheCamera.Yaw(-Speed, 0.1f);
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000)
		TheCamera.Yaw(Speed, 0.1f);
	if (::GetAsyncKeyState(VK_UP) & 0x8000)
		TheCamera.Pitch(-Speed, 0.1f);
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		TheCamera.Pitch(Speed, 0.1f);
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		TheCamera.Roll(Speed, 0.1f);
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		TheCamera.Roll(-Speed, 0.1f);
	
	static float Scale = 1.f;

	if (::GetAsyncKeyState('1') & 0x8000)
		Scale += Speed;
	if (::GetAsyncKeyState('2') & 0x8000)
		Scale -= Speed;

	int _ShadeMode = D3DSHADE_FLAT;
	
	if (::GetAsyncKeyState('3') & 0x8000)
		_ShadeMode = D3DSHADE_GOURAUD;
	if (::GetAsyncKeyState('4') & 0x8000)
		_ShadeMode = D3DSHADE_FLAT;
	if (::GetAsyncKeyState('5') & 0x8000)
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (::GetAsyncKeyState('6') & 0x8000)
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	constexpr double timeDelta = 1.f / 1000.f;
	
	Device->SetTransform(D3DTS_VIEW, &TheCamera.GetViewMatrix());
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET
			| D3DCLEAR_ZBUFFER, 0x0000ff, 1, 0);
		Device->BeginScene();
		_matrix world;
		D3DXMatrixIdentity(&world);
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetStreamSource(0, _TerrainVtxBuffer, 0, sizeof(TerrainVertex));
		Device->SetFVF(TerrainVertex::FVF);
		Device->SetRenderState(D3DRS_SHADEMODE, _ShadeMode);

		
		D3DVERTEXBUFFER_DESC _Dsc;
		_TerrainVtxBuffer->GetDesc(&_Dsc);
		DWORD VtxNum = _Dsc.Size / sizeof(TerrainVertex);
		D3DINDEXBUFFER_DESC _IdxDsc;
		_TerrainIdxBuffer->GetDesc(&_IdxDsc);
		UINT PrimCount = _IdxDsc.Size / 3;
		Device->SetRenderState(D3DRS_LIGHTING, false);
		Device->SetIndices(_TerrainIdxBuffer);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0, VtxNum,0 ,PrimCount);
		
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

	
	d3d::DrawBasicScene(0, 1.f);
}
