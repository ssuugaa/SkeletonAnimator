#include "stdafx.h"
#include "Mouse.h"

C_Mouse::C_Mouse(HWND handle)
	: position (0, 0)
	, handle(handle)
	, wheel(0)
{
	ZeroMemory(&buttonStatus, sizeof(buttonStatus));
	ZeroMemory(&buttonPrevStatus, sizeof(buttonPrevStatus));
	ZeroMemory(&buttonMap, sizeof(buttonMap));
	ZeroMemory(&startDoubleClickTime, sizeof(startDoubleClickTime));
	ZeroMemory(&buttonCount, sizeof(buttonCount));
	/*
	ZeroMemory(&buttonStatus, 8 * sizeof(BYTE));
	ZeroMemory(&buttonPrevStatus, 8 * sizeof(BYTE));
	ZeroMemory(&buttonMap, 8 * sizeof(Button));
	ZeroMemory(&startDoubleClickTime, 8 * sizeof(DWORD));
	ZeroMemory(&buttonCount, 8 * sizeof(int));
	*/
	doubleClickTime = GetDoubleClickTime();
}

C_Mouse::~C_Mouse()
{
}

void C_Mouse::Update()
{
	memcpy(buttonPrevStatus, buttonStatus, sizeof(buttonStatus));

	ZeroMemory(buttonStatus, sizeof(buttonStatus));
	ZeroMemory(buttonMap, sizeof(buttonMap));
	
	/*
	memcpy(buttonPrevStatus, buttonStatus, 8 * sizeof(BYTE));

	ZeroMemory(buttonStatus, 8 * sizeof(BYTE));
	ZeroMemory(buttonMap, 8 * sizeof(Button));
	*/
	buttonStatus[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	buttonStatus[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	buttonStatus[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;

	for (UINT i = 0; i < 8; i++)
	{
		BYTE prevStatus = buttonPrevStatus[i];
		BYTE currentStatus = buttonStatus[i];

		if (prevStatus == 0 && currentStatus == 1)
			buttonMap[i] = Button::Down;
		else if (prevStatus == 1 && currentStatus == 0)
			buttonMap[i] = Button::Up;
		else if (prevStatus == 1 && currentStatus == 1)
			buttonMap[i] = Button::Press;
	}

	// 더블클릭
	UINT currentTime = GetTickCount();
	for (UINT i = 0; i < 8; i++)
	{
		if (buttonMap[i] == Button::Down)
		{
			if (buttonCount[i] == 1)
			{
				if (currentTime - startDoubleClickTime[i] >= doubleClickTime)
					buttonCount[i] = 0;

			}

			buttonCount[i]++;

			if (buttonCount[i] == 1)
				startDoubleClickTime[i] = currentTime;
		}

		if (buttonMap[i] == Button::Up)
		{
			if (buttonCount[i] == 1)
			{
				if (currentTime - startDoubleClickTime[i] >= doubleClickTime)
					buttonCount[i] = 0;
			}
			else if (buttonCount[i] == 2)
			{
				if (currentTime - startDoubleClickTime[i] <= doubleClickTime)
					buttonMap[i] = Button::DoubleClick;
				buttonCount[i] = 0;
			}
		}
	} // for(i)

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(handle, &point);

	wheelMove = 0;
	wheelMove = wheel;
	wheel = 0;
}

void C_Mouse::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (msg == WM_MOUSEMOVE)
	{
		position.x = LOWORD(lParam);
		position.y = HIWORD(lParam);
	}
	if (msg == WM_MOUSEWHEEL)
	{
		short temp = (short)HIWORD(wParam);
		wheel = temp;
	}
	if(msg != WM_MOUSEWHEEL)
	{
		wheel = 0;
	}
}
