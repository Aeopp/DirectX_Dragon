#include "stdafx.h"
#include "..\Headers\Map.h"
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <map>

const DWORD CMap::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

CMap::CMap(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)  {}

HRESULT CMap::ReadyGameObjectPrototype()
{
	if (FAILED(CGameObject::ReadyGameObjectPrototype()))
		return E_FAIL;

	std::wstring _MtlFileName = L"..\\Resources\\Map\\Map01\\MAP.mtl";
	std::wfstream _MtlStream(_MtlFileName);

	if (!_MtlStream.is_open())
	{
		::MessageBox(nullptr, __FUNCTIONW__,
			L"File Open Fail", 0);
	}

	std::map<wstring, MtrlInfo> _MtrlInfo;
	std::wstring Line;
	std::wstringstream wss;
	std::wstring ToKen;

	while (_MtlStream)
	{
		Line.clear();
		ToKen.clear();
		// 한줄 리딩
		std::getline(_MtlStream, Line);
		wss.str(Line);
		wss.clear();
		wss >> ToKen;

		// 머테리얼 파싱 시작
		if (ToKen == L"newmtl")
		{
			MtrlInfo _Info;
			wss >> _Info.MtrlName;
			std::vector<std::wstring> _FileLines;

			while (true)
			{
				std::getline(_MtlStream, Line);
				_FileLines.push_back(Line);
				if (Line.empty())
					break;
			}

			std::reverse(std::begin(_FileLines), std::end(_FileLines));

			while (!_FileLines.empty())
			{
				// 한줄씩 읽으며 파싱
				ToKen.clear();
				std::wstringstream CurLine;
				CurLine.str(std::move(_FileLines.back()));
				_FileLines.pop_back();

				CurLine >> ToKen;

				if (ToKen == L"Kd")
				{
					CurLine >> _Info.Diffuse.r;
					CurLine >> _Info.Diffuse.g;
					CurLine >> _Info.Diffuse.b;
				}
				else if (ToKen == L"map_Kd")
				{
					CurLine >> _Info.TextureName;
				}
				else if (ToKen == L"Ks")
				{
					CurLine >> _Info.Specular.r;
					CurLine >> _Info.Specular.g;
					CurLine >> _Info.Specular.b;
				}
				else if (ToKen == L"Ns")
				{
					CurLine >> _Info.Shine;
				}
				else if (ToKen == L"illum")
				{
					CurLine >> (_uint&)_Info.Illu;
				}

				if (ToKen.empty())
				{

					_MtrlInfo.insert
					({
						_Info.MtrlName  , _Info
						});
				}
			}
		}
	}

	//Line.clear();
	//wss.str(L"");
	//wss.clear();
	//ToKen.clear();

	//std::wstring _ObjFileName = L"..\\Resources\\Map\\Map01\\MAP.obj";
	//std::wfstream _ObjStream(_ObjFileName);

	//if (!_ObjStream.is_open())
	//{
	//	::MessageBox(nullptr, __FUNCTIONW__,
	//		L"File Open Fail", 0);
	//}

	//Infos;

	//while (_ObjStream)
	//{
	//	std::getline(_ObjStream, Line);
	//	ToKen.clear();
	//	wss.str(Line);
	//	wss.clear();

	//	wss >> ToKen;

	//	if (ToKen == L"v")
	//	{

	//	}
	//}

	//int debug = 0;

	// 머테리얼 파싱 끝

	std::wstring _ObjFileName = L"..\\Resources\\Map\\Map01\\MAP.obj";

	const std::wstring V = L"v";
	const std::wstring Vt = L"vt";
	const std::wstring VN = L"vn";
	const std::wstring F = L"f";
	const std::wstring Mtl = L"usemtl";
	const wchar_t FDelim = L'\/';
	using _VtxType = Vertex;
	size_t VtxElementCount = 3;

	std::wfstream _ObjStream(_ObjFileName);

	if (!_ObjStream.is_open() )
		::MessageBox(nullptr, __FUNCTIONW__,
			L"File Open Fail", 0);

	std::vector<_VtxType> _Vtxs;
	std::vector<_vector> _Locations;
	std::vector<_vector> _Normals;
	std::vector<D3DXVECTOR2> _TextureCoords;

	wss.str(L"");
	wss.clear();
	Line.clear();

	std::wstring CurMtrlInfoName;

	while (_ObjStream)
	{
		// Line Read.
		std::getline(_ObjStream, Line);
		wss.str(Line);
		wss.clear();
		ToKen.clear();
		wss >> ToKen;

		if (ToKen == V)
		{
			_vector VertexLocation{ 0,0,0 };
			wss >> VertexLocation.x;
			wss >> VertexLocation.y;
			wss >> VertexLocation.z;
			_Locations.push_back(std::move(VertexLocation));
		}
		else if (ToKen == Vt)
		{
			D3DXVECTOR2 VertexTexCoord{ 0,0 };
			wss >> VertexTexCoord.x;
			wss >> VertexTexCoord.y;
			_TextureCoords.push_back(std::move(VertexTexCoord));
		}
		else if (ToKen == VN)
		{
			_vector VertexNormal{ 0,0,0 };
			wss >> VertexNormal.x;
			wss >> VertexNormal.y;
			wss >> VertexNormal.z;
			_Normals.push_back(std::move(VertexNormal));
		}
		else if (ToKen == Mtl)
		{
			std::wstring MtrlName;
			wss >> MtrlName;
			auto iter = _MtrlInfo.find(MtrlName);
			if (iter != std::end(_MtrlInfo))
			{
				CurMtrlInfoName = iter->first;
			}
		}
		else if (ToKen == F)
		{
			std::wstring ReplaceLine;
			std::getline(wss, ReplaceLine);
			std::replace_copy(std::begin(ReplaceLine), std::end(ReplaceLine),
				std::begin(ReplaceLine), FDelim, L' ');
			wss.str(ReplaceLine);
			wss.clear();
			for (size_t i = 0; i < 3; ++i)
			{
				_int LocationIdx, NormalIdx, TexCoordIdx;
				wss >> LocationIdx;
				wss >> TexCoordIdx;
				wss >> NormalIdx;

				_Vtxs.emplace_back
				(
					_VtxType
					{
						_Locations[LocationIdx - 1],
						_Normals[NormalIdx - 1],
						_TextureCoords[TexCoordIdx - 1]
					}
				);
			}
			ReplaceLine.clear();
		}

		if (ToKen.empty() && !_Vtxs.empty())
		{
			Info _Info;

			if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(Vertex) *
				_Vtxs.size(),
				0, Vertex::FVF, D3DPOOL_MANAGED,
				&_Info.VtxBuf, 0)))
			{
				MessageBox(nullptr, __FUNCTIONW__, nullptr, 0);
				return E_FAIL;
			}

			Vertex* _VtxBuffer;
			_Info.VtxBuf->Lock(0, 0, (void**)&_VtxBuffer, 0);

			memcpy(_VtxBuffer, _Vtxs.data(), sizeof(Vertex) * _Vtxs.size());
			_Info.TriangleCount = _Vtxs.size() / 3;
			_Vtxs.clear();
			_Info.VtxBuf->Unlock();

			auto iter = _MtrlInfo.find(CurMtrlInfoName);

			if (iter == std::end ( _MtrlInfo)  ) 
			{
				MessageBox(nullptr, L"	if (iter == std::end ( _MtrlInfo)  ) ", nullptr, 0);
				return E_FAIL;
			}

			_Info.MaterialInfo = iter->second;

			std::wstring TexName =L"..\\Resources\\Map\\Map01\\";
			TexName += _Info.MaterialInfo.TextureName;


			if (FAILED(D3DXCreateTextureFromFile(m_pDevice,
				TexName.c_str(), &_Info.Texture)))
			{
				return E_FAIL;
			}
			
			Infos.push_back(_Info);
		}

		
		wss.str(std::wstring{});
		Line.clear();
		wss.clear();
	}

	auto color = D3DXCOLOR(1, 1, 1, 1);

	SpotLight.Type = D3DLIGHT_SPOT;;
	SpotLight.Ambient = D3DXCOLOR(1.f, 1.0f, 1.0f, 1.0f);
	SpotLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.f, 1.f);
	SpotLight.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	SpotLight.Direction = D3DXVECTOR3(0.f, 0.f, 1.f);
	SpotLight.Position = { 0,0,0 };

	SpotLight.Range = 100000.f;
	SpotLight.Falloff = 1.0f;
	SpotLight.Attenuation0 = 1.0f;
	SpotLight.Attenuation1 = 0.0f;
	SpotLight.Attenuation2 = 0.0f;
	SpotLight.Theta = 0.8f;
	SpotLight.Phi = 1.6f;

	m_pDevice->SetLight(0, &SpotLight);
	m_pDevice->LightEnable(0, true);
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	/*if (FAILED(
		m_pDevice->CreateVertexBuffer(sizeof(Vertex) *
			_Vtxs.size(),
			0, Vertex::FVF, D3DPOOL_MANAGED,
			&_GunVtxBuffer, 0)))
	{
		MessageBox(nullptr, __FUNCTIONW__, nullptr, 0);
	}


	int PT_Count = _Vtxs.size() / 3;
	Vertex* _VtxBuffer;

	_GunVtxBuffer->Lock(0, 0, (void**)&_VtxBuffer, 0);
	memcpy(_VtxBuffer, _Vtxs.data(), sizeof(_VtxType) * _Vtxs.size());
	_GunVtxBuffer->Unlock();*/

	return S_OK;
}

HRESULT CMap::ReadyGameObject(void * pArg)
{
	if (FAILED(CGameObject::ReadyGameObject(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	return S_OK;
}

_uint CMap::UpdateGameObject(float fDeltaTime)
{
	CGameObject::UpdateGameObject(fDeltaTime);	

	auto pManagent = CManagement::Get_Instance();
	auto _Comp = pManagent->GetComponent(static_cast<int> (ESceneID::Stage), L"Layer_Player", L"Com_Transform", 0);
	auto* _Transform  = dynamic_cast<CTransform*>(_Comp);

	if (_Transform)
	{

		_vector Location = _Transform->GetTransformDesc().vPosition;
		SpotLight.Position = Location;

		auto rot = _Transform->GetTransformDesc().vRotation.y;
		_vector look = { 0,0,1 };
		_matrix rotm;
		D3DXMatrixRotationY(&rotm, rot);

		D3DXVec3TransformNormal(&look, &look,&rotm);

		SpotLight.Direction = look;

		m_pDevice->SetLight(0, &SpotLight);
		m_pDevice->LightEnable(0, true);
	}
	return _uint();
}

_uint CMap::LateUpdateGameObject(float fDeltaTime)
{
	CGameObject::LateUpdateGameObject(fDeltaTime);

	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return 0;

	if (FAILED(pManagement->AddGameObjectInRenderer(ERenderID::NoAlpha, this)))
		return 0;

	return _uint();
}

HRESULT CMap::RenderGameObject()
{
	if (FAILED(CGameObject::RenderGameObject()))
		return E_FAIL;

	const float scale = 10.f;

	_matrix world;
	D3DXMatrixIdentity(&world);
	world._33 = world._22   = world._11 = scale;
	world._41 = 20, world._42 = -10;
	m_pDevice->SetTransform(D3DTS_WORLD, &world); 

	for (auto& RefInfo : Infos)
	{
		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		m_pDevice->SetTexture(0, RefInfo.Texture);
		auto _Mtrl= RefInfo.MaterialInfo.ConvertMtrl();
		m_pDevice->SetMaterial(&_Mtrl);
		m_pDevice->SetStreamSource(0, RefInfo.VtxBuf, 0, 
			sizeof(Vertex));
		m_pDevice->SetFVF(Vertex::FVF);

		m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 
		RefInfo.TriangleCount);
	}


	return S_OK;
}

HRESULT CMap::AddComponent()
{
	
	return S_OK;
}

HRESULT CMap::Movement(float fDeltaTime)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	
	return S_OK;
}

CMap * CMap::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
		return nullptr;

	CMap* pInstance = new CMap(pDevice);

	if (FAILED(pInstance->ReadyGameObjectPrototype()))
	{
		PRINT_LOG(L"Warning", L"Failed To Create Map");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMap::Clone(void * pArg)
{
	CMap* pClone = new CMap(*this); /* 복사생성자 */

	if (FAILED(pClone->ReadyGameObject(pArg)))
	{
		PRINT_LOG(L"Warning", L"Failed To Clone Map");
		SafeRelease(pClone);
	}

	return pClone;
}

void CMap::Free()
{
	CGameObject::Free();
}
