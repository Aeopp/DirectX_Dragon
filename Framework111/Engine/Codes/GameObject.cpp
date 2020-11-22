#include "..\Headers\GameObject.h"

USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
{
	SafeAddRef(m_pDevice);
}

HRESULT CGameObject::ReadyGameObjectPrototype()
{
	return S_OK;
}

HRESULT CGameObject::ReadyGameObject(void * pArg)
{
	return S_OK;
}

_uint CGameObject::UpdateGameObject(float fDeltaTime)
{
	return _uint();
}

_uint CGameObject::LateUpdateGameObject(float fDeltaTime)
{
	return _uint();
}

HRESULT CGameObject::RenderGameObject()
{
	return S_OK;
}

void CGameObject::Free()
{
	SafeRelease(m_pDevice);
}
