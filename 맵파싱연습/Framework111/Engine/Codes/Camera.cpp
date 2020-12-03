#include "..\Headers\Camera.h"

USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_CameraDesc, sizeof(CAMERA_DESC));
	D3DXMatrixIdentity(&m_CameraDesc.matView);
	D3DXMatrixIdentity(&m_CameraDesc.matProj);
}

CCamera::CCamera(const CCamera & other)
	: CGameObject(other)
	, m_CameraDesc(other.m_CameraDesc)
{
}

HRESULT CCamera::ReadyGameObjectPrototype()
{
	if (FAILED(CGameObject::ReadyGameObjectPrototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCamera::ReadyGameObject(void * pArg/* = nullptr*/)
{
	if (FAILED(CGameObject::ReadyGameObject(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera::UpdateGameObject(float fDeltaTime)
{
	CGameObject::UpdateGameObject(fDeltaTime);

	

	return _uint();
}

_uint CCamera::LateUpdateGameObject(float fDeltaTime)
{
	CGameObject::LateUpdateGameObject(fDeltaTime);

	/* ����� ����� �Լ� */
	D3DXMatrixLookAtLH(
		&m_CameraDesc.matView, /* ��ȯ �� */
		&m_CameraDesc.vEye, /* ����� ī�޶� ��ġ Eye */
		&m_CameraDesc.vAt, /* ī�޶� �ٶ󺸴� ���� At */
		&m_CameraDesc.vUp /* Up���� */);

	if (FAILED(m_pDevice->SetTransform(D3DTS_VIEW, &m_CameraDesc.matView)))
		return E_FAIL;

	/* ���� ���� ����� ����� �Լ� */
	D3DXMatrixPerspectiveFovLH(
		&m_CameraDesc.matProj, /* ��ȯ �� */
		D3DXToRadian(m_CameraDesc.fFovY), /* �þ߰� FovY */
		m_CameraDesc.fAspect, /* ��Ⱦ�� Aspect */
		m_CameraDesc.fNear, /* ī�޶�� �������� Z�Ÿ� Near */
		m_CameraDesc.fFar /* ī�޶�� �������� Z�Ÿ� Far */);

	if (FAILED(m_pDevice->SetTransform(D3DTS_PROJECTION, &m_CameraDesc.matProj)))
		return E_FAIL;

	return _uint();
}

HRESULT CCamera::RenderGameObject()
{
	if (FAILED(CGameObject::RenderGameObject()))
		return E_FAIL;

	return S_OK;
}

void CCamera::Free()
{
	CGameObject::Free();
}
