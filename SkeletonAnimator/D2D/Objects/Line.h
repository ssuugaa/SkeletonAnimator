#pragma once

class Line
{
public:
	Line(Vector2 a, Vector2 b);
	~Line();

	void Update(Matrix& V, Matrix& P);
	void Render();

public:
	void Left(Vector2& vec) { left = vec; }
	void Right(Vector2& vec) { right = vec; }

	Vector2 Left() { return left; }
	Vector2 Right() { return right; }

public:
	void Position(float x, float y);
	void Position(Vector2& vec);
	void AddPosition(Vector2& vec);
	Vector2 Position() { return position; }

	void Scale(float x, float y);
	void Scale(Vector2& vec);
	Vector2 Scale() { return scale; }

	void Rotation(float z);
	void Rotation(Vector3& vec);
	Vector2 Rotation() { return rotation; }

	void Pass(int val) { pass = val; }

private:
	void CreateBuffer(Vector2 a, Vector2 b);
	void UpdateWorld();

private:
	struct Vertex
	{
		Vector3 Position;
	} vertices[2];

private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;

	Vector2 position;
	Vector2 scale;
	Vector3 rotation;

	Vector2 left;
	Vector2 right;

	ID3DX11EffectMatrixVariable* sWorld;
	ID3DX11EffectMatrixVariable* sView;
	ID3DX11EffectMatrixVariable* sProjection;

	int pass;
};