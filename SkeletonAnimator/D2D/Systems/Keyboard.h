#pragma once

#define MAX_KEY_COUNT 256

class Keyboard
{
public:
	Keyboard();
	~Keyboard();
	
	bool Down(int Key);
	bool Up(int Key);
	bool Press(int Key);
	bool Toggle(int Key);

private:
	bitset<MAX_KEY_COUNT> up;	// bool타입 배열 256개 만들었다.
	bitset<MAX_KEY_COUNT> down;

};

// 둘은 같다.
// up[65] = true;
// up.set(65, true);

/*
up.reset();	// 전부 false로 바꿔준다.
up.set();	// 전부 true로 바꿔준다.
up.set(int index, bool input);	//해당 위치의 값을 input으로 바꿔준다.
up.flip();	// 전부 반전 (true를 false로, false를 true로)
up.flip(int index);	// 해당 위치만 반전
up.all();	// 모두 true일때 true반환
up.none();	// 모두 false일때 true반환
up.count();	// true의 갯수를 세준다.
*/