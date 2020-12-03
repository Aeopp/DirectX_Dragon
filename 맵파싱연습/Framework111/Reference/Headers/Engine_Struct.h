#pragma once

#ifndef __ENGINE_STRUCT_H__

BEGIN(Engine)

typedef struct tagVertexColor
{
	D3DXVECTOR3 vPosition;
	_uint iColor;
}VTX_COLOR;

const _uint FVF_VTX_COLOR = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

typedef struct tagVertexTexture
{
	D3DXVECTOR3 vPosition;
	D3DXVECTOR2 vUV; /* �ؽ�ó ������ UV��ǥ ������ 0,0 ~ 1,1 */
}VTX_TEXTURE;

const _uint FVF_VTX_TEXTURE = D3DFVF_XYZ | D3DFVF_TEX1 /*| D3DFVF_TEXCOORDSIZE2(0)*/;

typedef struct tagIndex16
{
	WORD _1, _2, _3;
}INDEX16;

typedef struct tagIndex32
{
	DWORD _1, _2, _3;
}INDEX32;

typedef struct tagTransformDesc
{
	_vector	vPosition;
	_vector	vScale;
	_vector	vRotation; // For.Degree
	_matrix matWorld;

	float fSpeedPerSec = 0.f;
	float fRotatePerSec = 0.f;
}TRANSFORM_DESC;

typedef struct tagCameraDesc
{
	_matrix	matView;
	_vector vEye;
	_vector vAt;
	_vector vUp;

	_matrix matProj;
	float	fFovY;	// Degree
	float	fAspect;
	float	fNear;
	float	fFar;
}CAMERA_DESC;

END

#define __ENGINE_STRUCT_H__
#endif