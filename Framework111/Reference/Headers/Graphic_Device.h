#pragma once
#ifndef __GRAPHIC_DEVICE_H__

#include "Base.h"

BEGIN(Engine)
class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	CGraphic_Device();
	virtual ~CGraphic_Device() =default;
public:
	LPDIRECT3DDEVICE9 Get_Device() { return m_pDevice; }
	
public:
	HRESULT Ready_Graphic_Device(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eDisplayMode);
	void Render_Begin();
	void Render_End(HWND hWND = nullptr);

public:
	virtual void Free() override;

private:	
	LPDIRECT3D9 m_pSDK; // �׷���ī���� ������ �����ϰ� �����ϴ� ��ü. 
	LPDIRECT3DDEVICE9 m_pDevice; // �ְ� ���������� �׷��� ��ġ�� �����ϴ� ��ü. 
	// ��ġ�� �����ϴ� ����. 

	//1. ��ġ�� �����ϱ� ���� ��ü�� �����ϴ� ��ü(m_pSdk)�� ����. 
	//2. ��ġ�� ������ �����ؾ��Ѵ�. 
	//3. ���ؿ� �´� ��ġ�� �����ϱ� ���� ��ü ����. 

	/*
	com - component Object Model�� ����. 
	�츮�� ���� �ִ� ���̷�Ʈ������ ��ġ Ȥ�� ��� �����͸� �ٷ�� ��ɵ��� �ϳ��� ��ǰó�� �������ְ� �ִ�. 
	�׸��� ����ڴ� �� ��ǰ���� �̿��Ͽ� ��ġ ���� �����ϵ� ���α׷��� ������ ������ �Ѵ�. 
	*/
};
END

#define __GRAPHIC_DEVICE_H__
#endif