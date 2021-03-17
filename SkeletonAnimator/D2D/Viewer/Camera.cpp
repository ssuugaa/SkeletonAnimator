#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: position (0,0)
{
	D3DXMatrixIdentity(&view);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// View
	// D3DXVECTOR3 eye = D3DXVECTOR3(0, 0, 0); // 카메라의 위치
	// D3DXVECTOR3 at = D3DXVECTOR3(0, 0, 1); // 관찰 방향
	// D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0); // 위쪽 기준점(중심축 역할)
	// 임의로 축을 회전해 2개의 축이 같은 방향을 가리키게 되면 스케일이 0이 되어 캐릭터가 사라지는 짐벌락 현상이 발생한다.
	// normal(Vector3, Vector3)	- 노멀라이즈
	// dot(Vector3, Vector3)	- 내적
	// cross(Vector3, Vector3)	- 외적
	// 역행렬은 로컬좌표를 글로벌 좌표로 바꿀때 쓴다. -> 자식 * 부모^(-1) = 자식의 글로벌 좌표
	
	D3DXVECTOR3 eye = D3DXVECTOR3(position.x, position.y, 0); // 카메라의 위치
	D3DXVECTOR3 at = D3DXVECTOR3(0, 0, 1); // 관찰 방향
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0); // 위쪽 기준점(중심축 역할)

	D3DXMatrixLookAtLH(&view, &eye, &(eye + at), &up);
}

void Camera::Position(float x, float y)
{
	Position(Vector2(x, y));
}

void Camera::Position(Vector2 vec)
{
	position = vec;
}
