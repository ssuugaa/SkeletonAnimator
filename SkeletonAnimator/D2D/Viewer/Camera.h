#pragma once

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void Update();

	void Position(float x, float y);
	void Position(Vector2 vec);
	Vector2 Position() { return position; }

	Matrix View() { return view; }

protected:
	Vector2 position;

private:
	Matrix view;
};