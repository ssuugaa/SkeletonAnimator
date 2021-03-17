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
	bitset<MAX_KEY_COUNT> up;	// boolŸ�� �迭 256�� �������.
	bitset<MAX_KEY_COUNT> down;

};

// ���� ����.
// up[65] = true;
// up.set(65, true);

/*
up.reset();	// ���� false�� �ٲ��ش�.
up.set();	// ���� true�� �ٲ��ش�.
up.set(int index, bool input);	//�ش� ��ġ�� ���� input���� �ٲ��ش�.
up.flip();	// ���� ���� (true�� false��, false�� true��)
up.flip(int index);	// �ش� ��ġ�� ����
up.all();	// ��� true�϶� true��ȯ
up.none();	// ��� false�϶� true��ȯ
up.count();	// true�� ������ ���ش�.
*/