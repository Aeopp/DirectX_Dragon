#pragma once
#ifndef __MAP_H__

#include "GameObject.h"

USING(Engine)
class CMap final : public CGameObject
{
private:
	explicit CMap(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CMap() = default;
public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT ReadyGameObjectPrototype() override;
	virtual HRESULT ReadyGameObject(void * pArg = nullptr) override;
	virtual _uint UpdateGameObject(float fDeltaTime) override;
	virtual _uint LateUpdateGameObject(float fDeltaTime) override;
	virtual HRESULT RenderGameObject() override;
private:
	HRESULT AddComponent();
	HRESULT Movement(float fDeltaTime);
public:
	static CMap* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
private:
	D3DLIGHT9 SpotLight;
	_vector SpotLightLocation;

	using _vector2 = D3DXVECTOR2;
	struct Vertex
	{
		_vector Location;
		_vector Normal;
		_vector2 TexCoord;
		static const DWORD FVF;
	};

	struct MtrlInfo
	{
		enum class Illumination
		{

		};
		D3DXCOLOR Ambient{ 0,0,0,1 };
		D3DXCOLOR Diffuse = { 0,0,0,1 };
		D3DXCOLOR Specular{ 0,0,0,1 };
		float Shine{ 0 };
		std::wstring TextureName{};
		std::wstring MtrlName{};
		Illumination Illu;

		D3DMATERIAL9 ConvertMtrl()
		{
			D3DMATERIAL9 _Mtrl;
			_Mtrl.Ambient = this->Ambient;
			_Mtrl.Diffuse = this->Diffuse;
			_Mtrl.Power = this->Shine;
			_Mtrl.Specular= Specular;


			return _Mtrl;
		}
	};

	struct Info
	{
		uint32_t TriangleCount = 0;
		IDirect3DVertexBuffer9* VtxBuf;
		MtrlInfo MaterialInfo;
		IDirect3DTexture9* Texture;
	};

	std::vector<Info> Infos;
};

#define __MAP_H__
#endif