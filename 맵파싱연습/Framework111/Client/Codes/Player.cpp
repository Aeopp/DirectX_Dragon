#include "stdafx.h"
#include "..\Headers\Player.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlayer::CPlayer(const CPlayer & other)
	: CGameObject(other)	
{
}

HRESULT CPlayer::ReadyGameObjectPrototype()
{
	if (FAILED(CGameObject::ReadyGameObjectPrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::ReadyGameObject(void * pArg)
{
	if (FAILED(CGameObject::ReadyGameObject(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	return S_OK;
}

_uint CPlayer::UpdateGameObject(float fDeltaTime)
{
	CGameObject::UpdateGameObject(fDeltaTime);	

	if (FAILED(Movement(fDeltaTime)))
		return 0;

	m_pTransformCom->UpdateTransform();

	return _uint();
}

_uint CPlayer::LateUpdateGameObject(float fDeltaTime)
{
	CGameObject::LateUpdateGameObject(fDeltaTime);

	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return 0;

	if (FAILED(pManagement->AddGameObjectInRenderer(ERenderID::NoAlpha, this)))
		return 0;

	return _uint();
}

HRESULT CPlayer::RenderGameObject()
{
	if (FAILED(m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransformCom->GetTransformDesc().matWorld)))
		return E_FAIL;	

	if (FAILED(CGameObject::RenderGameObject()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render_VIBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::AddComponent()
{
	/* For.Com_Texture */
	if (FAILED(CGameObject::AddComponent(
		(_int)ESceneID::Static,
		L"Component_Texture_Player",
		L"Com_Texture",
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::AddComponent(
		(_int)ESceneID::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TRANSFORM_DESC));
	TransformDesc.vPosition = _vector(5.f, 5.f, 0.f);
	TransformDesc.vScale = _vector(1.f, 1.f, 1.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotatePerSec = 90.f;

	if (FAILED(CGameObject::AddComponent(
		(_int)ESceneID::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransformCom, 
		&TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Movement(float fDeltaTime)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransformCom->GoStraight(fDeltaTime);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransformCom->GoStraight(-fDeltaTime);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pTransformCom->GoSide(-fDeltaTime);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pTransformCom->GoSide(fDeltaTime);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransformCom->AddRotationY(-fDeltaTime);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransformCom->AddRotationY(fDeltaTime);
	}

	//CVIBuffer_TerrainTexture* pTerrainBuffer = (CVIBuffer_TerrainTexture*)pManagement->GetComponent((_int)ESceneID::Stage, L"Layer_Terrain", L"Com_VIBuffer");
	//if (nullptr == pTerrainBuffer)
	//	return E_FAIL;

	//_vector vPosition = m_pTransformCom->GetTransformDesc().vPosition;
	//if (true == pTerrainBuffer->IsCollision(&vPosition))
	//{
	//	m_pTransformCom->SetPosition(vPosition);
	//}
	
	return S_OK;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
		return nullptr;

	CPlayer* pInstance = new CPlayer(pDevice);
	if (FAILED(pInstance->ReadyGameObjectPrototype()))
	{
		PRINT_LOG(L"Warning", L"Failed To Create Player");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void * pArg)
{
	CPlayer* pClone = new CPlayer(*this); /* 복사생성자 */
	if (FAILED(pClone->ReadyGameObject(pArg)))
	{
		PRINT_LOG(L"Warning", L"Failed To Clone Player");
		SafeRelease(pClone);
	}

	return pClone;
}

void CPlayer::Free()
{
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);

	CGameObject::Free();
}
