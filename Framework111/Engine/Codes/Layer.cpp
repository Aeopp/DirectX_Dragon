#include "..\Headers\Layer.h"
#include "GameObject.h"

USING(Engine)

CLayer::CLayer()
{
}

HRESULT CLayer::AddGameObjectInLayer(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	auto iter_find = find(m_GameObjects.begin(), m_GameObjects.end(), pGameObject);
	if (m_GameObjects.end() != iter_find)
	{
		PRINT_LOG(L"Warning", L"Already exist in current layer");
		return E_FAIL;
	}

	return S_OK;
}

_uint CLayer::UpdateGameObject(float fDeltaTime)
{
	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->UpdateGameObject(fDeltaTime);
	}

	return _uint();
}

_uint CLayer::LateUpdateGameObject(float fDeltaTime)
{
	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->LateUpdateGameObject(fDeltaTime);
	}

	return _uint();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
	{
		SafeRelease(pGameObject);
	}

	m_GameObjects.clear();
}
