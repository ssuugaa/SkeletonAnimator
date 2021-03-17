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
	// D3DXVECTOR3 eye = D3DXVECTOR3(0, 0, 0); // ī�޶��� ��ġ
	// D3DXVECTOR3 at = D3DXVECTOR3(0, 0, 1); // ���� ����
	// D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0); // ���� ������(�߽��� ����)
	// ���Ƿ� ���� ȸ���� 2���� ���� ���� ������ ����Ű�� �Ǹ� �������� 0�� �Ǿ� ĳ���Ͱ� ������� ������ ������ �߻��Ѵ�.
	// normal(Vector3, Vector3)	- ��ֶ�����
	// dot(Vector3, Vector3)	- ����
	// cross(Vector3, Vector3)	- ����
	// ������� ������ǥ�� �۷ι� ��ǥ�� �ٲܶ� ����. -> �ڽ� * �θ�^(-1) = �ڽ��� �۷ι� ��ǥ
	
	D3DXVECTOR3 eye = D3DXVECTOR3(position.x, position.y, 0); // ī�޶��� ��ġ
	D3DXVECTOR3 at = D3DXVECTOR3(0, 0, 1); // ���� ����
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0); // ���� ������(�߽��� ����)

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
