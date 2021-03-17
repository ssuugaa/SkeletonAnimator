#pragma once

class Freedom : public Camera
{
public:
	Freedom(float speed = 200.0f);
	~Freedom();

	void Update() override;

	void Speed(float speed) { this->speed = speed; }

private:
	void Move(Vector2 translation);

private:
	float speed;

};