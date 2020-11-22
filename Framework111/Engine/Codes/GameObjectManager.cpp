#include "..\Headers\GameObjectManager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGameObjectManager)

CGameObjectManager::CGameObjectManager()
{
}

HRESULT CGameObjectManager::ReserveSizePrototypeContainer(_int iSceneCount)
{
	if (0 > iSceneCount)
		return E_FAIL;

	m_iSceneCount = iSceneCount;
	m_pPrototypes = new PROTOTYPES[m_iSceneCount];
	if (nullptr == m_pPrototypes)
	{
		PRINT_LOG(L"Error", L"Failed To ReserveSizePrototypeContainer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameObjectManager::AddGameObjectPrototype(
	_int iSceneIndex, 
	const wstring & GameObjectTag, 
	CGameObject * pPrototype)
{
	if (0 > iSceneIndex ||
		m_iSceneCount <= iSceneIndex)
	{
		PRINT_LOG(L"Error", L"Out of range GameObject PrototypeContainer");
		return E_FAIL;
	}

	auto iter_find = m_pPrototypes[iSceneIndex].find(GameObjectTag);
	if (m_pPrototypes[iSceneIndex].end() == iter_find)
	{
		m_pPrototypes[iSceneIndex].insert(make_pair(GameObjectTag, pPrototype));
	}

	return S_OK;
}

CGameObject * CGameObjectManager::CloneGameObjectPrototype(
	_int iSceneIndex, 
	const wstring & GameObjectTag,
	void * pArg)
{
	if (0 > iSceneIndex ||
		m_iSceneCount <= iSceneIndex)
	{
		PRINT_LOG(L"Error", L"Out of range GameObject PrototypeContainer");
		return nullptr;
	}

	auto iter_find = m_pPrototypes[iSceneIndex].find(GameObjectTag);
	if (m_pPrototypes[iSceneIndex].end() == iter_find)
	{
		TCHAR szBuffer[128] = L"";
		swprintf_s(szBuffer, L"Not found %s Prototype", GameObjectTag.c_str());

		PRINT_LOG(L"Warning", szBuffer);
		return nullptr;
	}

	return iter_find->second->Clone(pArg);
}

HRESULT CGameObjectManager::ClearForScene(_int iSceneIndex)
{
	if (0 > iSceneIndex ||
		m_iSceneCount <= iSceneIndex)
	{
		PRINT_LOG(L"Error", L"Out of range GameObject PrototypeContainer");
		return E_FAIL;
	}

	for (auto& Pair : m_pPrototypes[iSceneIndex])
	{
		SafeRelease(Pair.second);
	}

	m_pPrototypes[iSceneIndex].clear();

	return S_OK;
}

_uint CGameObjectManager::UpdateGameObject(float fDeltaTime)
{
	return _uint();
}

_uint CGameObjectManager::LateUpdateGameObject(float fDeltaTime)
{
	return _uint();
}

void CGameObjectManager::Free()
{
	for (_int i = 0; i < m_iSceneCount; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
		{
			SafeRelease(Pair.second);
		}
		m_pPrototypes[i].clear();
	}

	SafeDeleteArray(m_pPrototypes);
}
