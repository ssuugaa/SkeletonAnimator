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

	// 아래 2가지 방법은 부모의 함수를 부르는 방법이다. 결과는 같음
	//Camera::Update();
	__super::Update();
}

void Freedom::Move(Vector2 translation)
{
	position += translation * Time::Delta();
}
