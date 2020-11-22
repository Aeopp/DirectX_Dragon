#pragma once

// TODO :: 이동,회전 할때에 델타타임을 받아오도록 수정해주세요.
class Camera
{
public :
	enum class EType : uint8_t{LANDOBJECT,FlyingObject};
	virtual ~Camera()noexcept = default;
public :
	void Pitch(const float Degree,const float DeltaTime);
	void Yaw(const float Degree, const float DeltaTime);
	void Roll(const float Degree, const float DeltaTime);
	_matrix GetViewMatrix();
public :
	FORCEINLINE void SetCameraType(EType _Type);
	FORCEINLINE _vector GetLocation();
	FORCEINLINE void SetLocation(const _vector Location);
	FORCEINLINE _vector GetRight();
	FORCEINLINE _vector GetUp();
	FORCEINLINE _vector GetLook();
public:
	/// <summary>
	/// 비행 물체가 아니라면 Y축 이동을 제한.
	/// </summary>
	void MoveForward(const float Speed,const float DeltaTime);
	void MoveUp(const float Speed,const float DeltaTime);
	void MoveRight(const float Speed,const float DeltaTime);
private:
	EType _Type = EType::FlyingObject;
	_vector _Up{ 0,1,0 }; 
	_vector _Look{ 0,0,1 };
	_vector _Right{ 1,0,0 };
	_vector _Location{ 0,0,0 };
};


void Camera::SetCameraType(EType _Type)
{
	this->_Type = _Type;
}

_vector Camera::GetLocation()
{
	return _Location;
}

void Camera::SetLocation(const _vector Location)
{
	_Location = std::move(Location);
}

_vector Camera::GetRight()
{
	return _Right;
}

_vector Camera::GetUp()
{
	return _Up;
}

_vector Camera::GetLook()
{
	return _Look;
}



