#include "stdafx.h"
#include "..\Headers\MainCamera.h"
#include "Player.h"

CMainCamera::CMainCamera(LPDIRECT3DDEVICE9 pDevice)
	: CCamera(pDevice)
{
}

CMainCamera::CMainCamera(const CMainCamera & other)
	: CCamera(other)
	, m_fDistanceToTarget(other.m_fDistanceToTarget)
	, m_fCameraAngle(other.m_fCameraAngle)
{
}

HRESULT CMainCamera::ReadyGameObjectPrototype()
{
	if (FAILED(CCamera::ReadyGameObjectPrototype()))
		return E_FAIL;

	/* 평면의 a, b, c, d 정보를 구해주는 함수. */
	//D3DXPlaneFromPoints()

	return S_OK;
}

HRESULT CMainCamera::ReadyGameObject(void * pArg/* = nullptr*/)
{
	if (FAILED(CCamera::ReadyGameObject(pArg)))
		return E_FAIL;

	if (pArg)
	{
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERA_DESC));
	}

	return S_OK;
}

_uint CMainCamera::UpdateGameObject(float fDeltaTime)
{
	CCamera::UpdateGameObject(fDeltaTime);

	if (FAILED(KeyInput(fDeltaTime)))
		return 0;

	if (FAILED(Movement(fDeltaTime)))
		return 0;

	return _uint();
}

_uint CMainCamera::LateUpdateGameObject(float fDeltaTime)
{
	CCamera::LateUpdateGameObject(fDeltaTime);

	return _uint();
}

HRESULT CMainCamera::RenderGameObject()
{
	if (FAILED(CCamera::RenderGameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::KeyInput(float fDeltaTime)
{
	if (GetAsyncKeyState('I') & 0x8000)
	{
		m_fDistanceToTarget -= 10.f * fDeltaTime;
	}

	if (GetAsyncKeyState('K') & 0x8000)
	{
		m_fDistanceToTarget += 10.f * fDeltaTime;
	}

	if (GetAsyncKeyState('O') & 0x8000)
	{
		m_fCameraAngle += 45.f * fDeltaTime;
	}

	if (GetAsyncKeyState('L') & 0x8000)
	{
		m_fCameraAngle -= 45.f * fDeltaTime;
	}

	return S_OK;
}

HRESULT CMainCamera::Movement(float fDeltaTime)
{
	auto pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CTransform* pPlayerTransform = (CTransform*)pManagement->GetComponent((_int)ESceneID::Stage, L"Layer_Player", L"Com_Transform");
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	/* 카메라는 플레이어를 바라봐야함 */
	m_CameraDesc.vAt = pPlayerTransform->GetTransformDesc().vPosition;

	/* 카메라는 일정거리만큼 플레이어 뒤에 배치 */
	_vector vPlayerLook;	
	memcpy(&vPlayerLook, &pPlayerTransform->GetTransformDesc().matWorld.m[2][0], sizeof(_vector));
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	/* 카메라는 현재 플레이어가 바라보는 방향과 반대에 
	설정한 거리만큼 떨어져 있어야 함. */
	vPlayerLook *= -m_fDistanceToTarget;

	/* vPlayerLook을 플레이어의 Right축 기준으로 회전시켜준다. */
	_vector vPlayerRight;
	memcpy(&vPlayerRight, &pPlayerTransform->GetTransformDesc().matWorld.m[0][0], sizeof(_vector));
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	_matrix matRotAxis;

	/* 임의의 축 기준으로 회전 */
	D3DXMatrixRotationAxis(&matRotAxis, &vPlayerRight, D3DXToRadian(m_fCameraAngle));
	D3DXVec3TransformNormal(&vPlayerLook, &vPlayerLook, &matRotAxis);

	m_CameraDesc.vEye = m_CameraDesc.vAt + vPlayerLook;	

	return S_OK;
}

CMainCamera * CMainCamera::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMainCamera* pInstance = new CMainCamera(pDevice);
	if (FAILED(pInstance->ReadyGameObjectPrototype()))
	{
		PRINT_LOG(L"Warning", L"Failed To Create Main Camera");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCamera::Clone(void * pArg/* = nullptr*/)
{
	CMainCamera* pClone = new CMainCamera(*this);
	if (FAILED(pClone->ReadyGameObject(pArg)))
	{
		PRINT_LOG(L"Warning", L"Failed To Clone Main Camera");
		SafeRelease(pClone);
	}

	return pClone;
}

void CMainCamera::Free()
{
	CCamera::Free();
}
