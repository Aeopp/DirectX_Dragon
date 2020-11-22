#pragma once
#ifndef __STAGE_H__

#include "Scene.h"

USING(Engine)
class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage() = default;

public:
	virtual HRESULT ReadyScene() override;
	virtual _uint UpdateScene() override;
	virtual _uint LateUpdateScene() override;

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;
private:
	void Initialize() & noexcept;
	void Render()&;
};

#define __STAGE_H__
#endif
