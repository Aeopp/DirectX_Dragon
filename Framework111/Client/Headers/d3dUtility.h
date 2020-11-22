//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dUtility.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include <d3dx9.h>
#include <string>
#include <type_traits>
#include <numeric>

namespace d3d
{
	bool InitD3D(
		HINSTANCE hInstance,       // [in] Application instance.
		int width, int height,     // [in] Backbuffer dimensions.
		bool windowed,             // [in] Windowed (true)or full screen (false).
		D3DDEVTYPE deviceType,     // [in] HAL or REF
		IDirect3DDevice9** device);// [out]The created device.

	int EnterMsgLoop( 
		bool (*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg, 
		WPARAM wParam,
		LPARAM lParam);
	
	template<class T> void Release(T& t)
	{
		if(t)
		{
			t->Release();
			t = 0;
		}
	}
		
	template<class T> void Delete(T t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}

	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

	//
	// Lights
	//

	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

	//
	// Materials
	//

	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

	const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	constexpr float Infinity = std::numeric_limits<float>::infinity();
	constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	template<class T>
	bool Equals(const T x,const T y)
	{
		static_assert(std::is_floating_point_v<T>, __FUNCTION__);
		
		return std::fabs(x - y) <= std::numeric_limits<T>::epsilon()
			
		|| std::fabs(x - y) < (std::numeric_limits<T>::min)();
	}

	
	struct BoundingBox
	{
		explicit BoundingBox();

		bool isPointInside(_vector& p);

		_vector _min {0,0,0};
		_vector _max{ 0,0,0 } ;

		
	};

	struct BoundingSphere
	{
		BoundingSphere();

		static bool ComputeBoundingSphere(ID3DXMesh* mesh)
		{
			HRESULT hr = 0;
			_vector* v = 0;
			BoundingSphere sphere;
			mesh->LockVertexBuffer(0, (void**)&v);

			hr = D3DXComputeBoundingSphere(
				(_vector*)v,
				mesh->GetNumVertices(),
				D3DXGetFVFVertexSize(mesh->GetFVF()),
				&sphere._center,
				&sphere._radius);

			mesh->UnlockVertexBuffer();

			if (FAILED(hr))
				return false;

			return true; 
		}
		
		_vector _center{ 0,0,0 } ;
		float _radius { 0 } ;
	};
 }

 d3d::BoundingBox::BoundingBox()
 {
	 _min.x = d3d::Infinity;
	 _min.y = d3d::Infinity;
	 _min.z = d3d::Infinity;

	 _max.x = -d3d::Infinity;
	 _max.y = -d3d::Infinity;
	 _max.z = -d3d::Infinity;
	
 }

 bool d3d::BoundingBox::isPointInside(_vector& p)
 {
	 if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		 p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
 }

 d3d::BoundingSphere::BoundingSphere()
 {
	 _radius = 0.0f;
 }


#endif // __d3dUtilityH__