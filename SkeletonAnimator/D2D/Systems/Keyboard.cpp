#include "stdafx.h"
#include "Keyboard.h"

Keyboard::Keyboard()
{
	up.reset();
	down.reset();
}

Keyboard::~Keyboard()
{
}

bool Keyboard::Down(int Key)
{
	if (GetAsyncKeyState(Key) & 0x8000)
	{
		if (!down[Key])
		{
			down.set(Key, true);
			return true;
		}
	}
	else
	{
		down.set(Key, false);
	}
	
	return false;
}

bool Keyboard::Up(int Key)
{
	if (GetAsyncKeyState(Key) & 0x8000)
	{
		up.set(Key, true);
	}
	else
	{
		if (up[Key])
		{
			up.set(Key, false);
			return true;
		}
	}

	return false;
}

bool Keyboard::Press(int Key)
{
	if (GetAsyncKeyState(Key) & 0x8000)
		return true;
	
	return false;
}

bool Keyboard::Toggle(int Key)
{
	if (GetAsyncKeyState(Key) & 0x8001)
		return true;

	return false;
}
