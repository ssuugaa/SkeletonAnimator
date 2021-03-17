#include "stdafx.h"
#include "Freedom.h"

Freedom::Freedom(float speed)
	: speed(speed)
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	if (Key->Press('D'))
		Move(Vector2(+1, 0) * speed);
	else if (Key->Press('A'))
		Move(Vector2(-1, 0) * speed);
	
	if (Key->Press('W'))
		Move(Vector2(0, +1) * speed);
	else if (Key->Press('S'))
		Move(Vector2(0, -1) * speed);

	// �Ʒ� 2���� ����� �θ��� �Լ��� �θ��� ����̴�. ����� ����
	//Camera::Update();
	__super::Update();
}

void Freedom::Move(Vector2 translation)
{
	position += translation * Time::Delta();
}
