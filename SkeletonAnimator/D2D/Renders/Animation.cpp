#include "stdafx.h"
#include "Animation.h"

Animation::Animation()
	: position(0, 0)
	, scale(1, 1)
	, rotation (0, 0, 0)
	, currentClip(-1)
{
}

Animation::~Animation()
{
	for (Clip* clip : clips)
		SafeDelete(clip);
}

void Animation::Update(Matrix & V, Matrix & P)
{
	if (currentClip < 0) return;

	clips[currentClip]->Position(position);
	clips[currentClip]->Scale(scale);
	clips[currentClip]->Rotation(rotation);
	
	clips[currentClip]->Update(V, P);
}

void Animation::Render()
{
	if (currentClip < 0) return;

	clips[currentClip]->Render();
}

void Animation::AddClip(Clip * clip)
{
	clips.push_back(clip);
}

void Animation::Play(UINT clipNumber)
{
	if (currentClip == clipNumber) return;

	if (clipNumber > -1)
		clips[currentClip]->Stop();

	currentClip = clipNumber;
	clips[currentClip]->Play();
}

void Animation::Stop()
{
	if (currentClip > -1)
		clips[currentClip]->Stop();
}

void Animation::Position(float x, float y)
{
	Position(Vector2(x, y));
}

void Animation::Position(Vector2 & vec)
{
	position = vec;
}

void Animation::Scale(float x, float y)
{
	Scale(Vector2(x, y));
}

void Animation::Scale(Vector2 & vec)
{
	scale = vec;
}

void Animation::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Animation::Rotation(Vector3 & vec)
{
	rotation = vec;
}

void Animation::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Animation::RotationDegree(Vector3 & vec)
{
	Vector3 temp;
	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

Vector3 Animation::RotationDegree()
{
	Vector3 temp;
	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

Vector2 Animation::TextureSize()
{
	assert(currentClip > -1);
	// 이렇게 최대값을 얻을 수 있다.
	// FLT_MAX;
	// INT_MAX;
	// UINT_MAX;
	// USHRT_MAX;

	return clips[currentClip]->TextureSize();
}

Clip * Animation::GetClip()
{
	if (currentClip < 0)
		return NULL;

	return clips[currentClip];
}

Sprite * Animation::GetSprite()
{
	return GetClip()->CurrentSprite();
}

void Animation::Pass(UINT val)
{
	for (Clip* clip : clips)
		clip->Pass(val);
}
