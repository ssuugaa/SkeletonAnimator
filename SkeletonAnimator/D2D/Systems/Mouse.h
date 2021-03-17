#pragma once

class C_Mouse
{
private:
	enum class Button{ None, Down, Up, Press, DoubleClick };

public:
	C_Mouse(HWND handle);
	~C_Mouse();

	void Update();

	void WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	
	bool Down(UINT button) { return buttonMap[button] == Button::Down; }
	bool Up(UINT button) { return buttonMap[button] == Button::Up; }
	bool Press(UINT button) { return buttonMap[button] == Button::Press; }
	bool DoubleClick(UINT button) { return buttonMap[button] == Button::DoubleClick; }
	
	Vector2 Position() { return position; }

	int Wheel() { return wheelMove | wheel; }

private:
	HWND handle;

	Vector2 position;
	// 마우스 왼쪽 = 0, 마우스 오른쪽 = 1, 마우스 휠 = 2
	// 마우스가 어떻게 변할지 모르니 넉넉잡고 8개 공간 만듬
	BYTE buttonStatus[8];	// 1바이트 정수타입 0~255	
	BYTE buttonPrevStatus[8];
	Button buttonMap[8];

	DWORD doubleClickTime;
	DWORD startDoubleClickTime[8];
	int buttonCount[8];

	int wheel;
	int wheelMove;
};