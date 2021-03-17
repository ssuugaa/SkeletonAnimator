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
	// ���콺 ���� = 0, ���콺 ������ = 1, ���콺 �� = 2
	// ���콺�� ��� ������ �𸣴� �˳���� 8�� ���� ����
	BYTE buttonStatus[8];	// 1����Ʈ ����Ÿ�� 0~255	
	BYTE buttonPrevStatus[8];
	Button buttonMap[8];

	DWORD doubleClickTime;
	DWORD startDoubleClickTime[8];
	int buttonCount[8];

	int wheel;
	int wheelMove;
};