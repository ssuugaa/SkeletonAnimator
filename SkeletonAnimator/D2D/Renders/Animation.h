#pragma once

class Animation
{
public:
	Animation();
	~Animation();

	void Update(Matrix& V, Matrix& P);
	void Render();

	void AddClip(Clip* clip);
	void Play(UINT clipNumber);
	void Stop();

	void Position(float x, float y);
	void Position(Vector2& vec);
	Vector2 Position() { return position; }

	void Scale(float x, float y);
	void Scale(Vector2& vec);
	Vector2 Scale() { return scale; }

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	Vector3 Rotation() { return rotation; }

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	Vector3 RotationDegree();

	Vector2 TextureSize();
	int GetCurrentClip() { return currentClip; }
	Clip* GetClip();
	Sprite* GetSprite();

	void Pass(UINT val);

private:
	Vector2 position;
	Vector2 scale;
	Vector3 rotation;

	int currentClip;	// 현재 클립 번호. -1일때 재생중인 클립 없음
	vector<Clip*> clips;
};