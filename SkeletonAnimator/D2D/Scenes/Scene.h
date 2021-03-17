#pragma once

#include "stdafx.h"

struct SceneValues
{
	Camera* MainCamera;
	Matrix Projection;
};

class Scene
{
public:

	Scene(SceneValues* values) { this->values = values; }
	virtual ~Scene() = default;

	virtual void Update() = 0;
	virtual void Render() = 0;

protected:
	SceneValues* values;
};