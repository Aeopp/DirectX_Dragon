#include "stdafx.h"
#include "Camera.h"


 void Camera::Pitch(const float Degree, const float DeltaTime)
{
	 _matrix Rot;
	 D3DXMatrixRotationAxis(&Rot, &_Right, Degree *DeltaTime);
	 D3DXVec3TransformCoord(&_Up, &_Up, &Rot);
	 D3DXVec3TransformCoord(&_Look, &_Look, &Rot);
}

 void Camera::Yaw(const float Degree, const float DeltaTime)
{
	 _matrix Rot;
 	
	 switch (_Type)
	 {
	 case EType::FlyingObject:
		 D3DXMatrixRotationAxis(&Rot, &_Up, Degree * DeltaTime);
		 break;
	 case EType::LANDOBJECT:
		 D3DXMatrixRotationY(&Rot, Degree * DeltaTime);
		 break;
	 }

	 D3DXVec3TransformCoord(&_Look, &_Look, &Rot);
	 D3DXVec3TransformCoord(&_Right, &_Right, &Rot);
}

 void Camera::Roll(const float Degree, const float DeltaTime)
{
	_matrix Rot;
	 switch (_Type)
	 {
	 case EType::FlyingObject:
		 D3DXMatrixRotationAxis(&Rot, &_Look, Degree * DeltaTime);
		 D3DXVec3TransformCoord(&_Up, &_Up, &Rot);
		 D3DXVec3TransformCoord(&_Right, &_Right, &Rot);
		 break;
	 case EType::LANDOBJECT: 
		 break;
	 }
}

 _matrix Camera::GetViewMatrix()
 {
	 _matrix View;
 	
	 D3DXVec3Normalize(&_Look, &_Look);

	 D3DXVec3Cross(&_Up, &_Look, &_Right);
	 D3DXVec3Normalize(&_Up, &_Up);

	 D3DXVec3Cross(&_Right, &_Up, &_Look);
	 D3DXVec3Normalize(&_Right, &_Right);
 	
	 const float x = -D3DXVec3Dot(&_Right, &_Location);
	 const float y = -D3DXVec3Dot(&_Up, &_Location);
	 const float z = -D3DXVec3Dot(&_Look, &_Location);

	 View(0, 0) = _Right.x; View(0, 1) = _Up.x; View(0, 2) = _Look.x; View(0, 3) = 0.0f;
	 View(1, 0) = _Right.y; View(1, 1) = _Up.y; View(1, 2) = _Look.y; View(1, 3) = 0.0f;
	 View(2, 0) = _Right.z; View(2, 1) = _Up.z; View(2, 2) = _Look.z; View(2, 3) = 0.0f;
	 View(3, 0) = x;        View(3, 1) = y;     View(3, 2) = z;       View(3, 3) = 1.0f;
	 return View;
 	
 };

void Camera::MoveForward(const float Speed,const float DeltaTime)
{
	_vector Dir;
	D3DXVec3Cross(&Dir, &_Right, &_Up);
	D3DXVec3Normalize(&Dir, &Dir);
	
	switch (_Type)
	{
	case EType::FlyingObject:
		_Location += Dir * Speed * DeltaTime;
		break;
	case EType::LANDOBJECT:
		_Location += _vector(Dir.x, 0, Dir.z) * Speed * DeltaTime;
		break;
	}
}

void Camera::MoveUp(const float Speed, const float DeltaTime)
{
	_vector Dir;
	D3DXVec3Cross(&Dir, &_Look, &_Right);
	D3DXVec3Normalize(&Dir, &Dir);
	
	switch (_Type)
	{
	case EType::FlyingObject:
		_Location += Dir * Speed * DeltaTime;
		break;
	case EType::LANDOBJECT:
		_Location.y += Speed * DeltaTime;
		break;
	}
}

void Camera::MoveRight(const float Speed, const float DeltaTime)
{
	_vector Dir;
	D3DXVec3Cross(&Dir, &_Up , &_Look);
	D3DXVec3Normalize(&Dir, &Dir);

	switch (_Type)
	{
	case EType::FlyingObject:
		_Location += Dir * Speed * DeltaTime;
		break;
	case EType::LANDOBJECT:
		_Location += _vector(Dir.x, 0, Dir.z) * Speed * DeltaTime;
		break;
	}
}
