#pragma once

class Sprite
{
public:
	Sprite(wstring textureFile, wstring shaderFile);
	Sprite(wstring textureFile, wstring shaderFile, float endX, float endY);
	Sprite(wstring textureFile, wstring shaderFile, float startX, float startY, float endX, float endY);

	Sprite(wstring textureFile, wstring shaderFile, Vector2 position, Vector2 scale = {0, 0});
	~Sprite();

	void Update(Matrix& V, Matrix& P);
	void Render();

	void Pass(UINT val) { pass = val; }
	Shader* GetShader() { return shader; }

	void Direction(float z) { direction.z = z; D3DXMatrixRotationYawPitchRoll(&D, direction.x, direction.y, direction.z); }
	float Direction() { return direction.z; }

	void Position(float x, float y);
	void Position(Vector2 vec);
	void AddPosition(Vector2 vec);
	Vector2 Position() { return position; }

	void Scale(float x, float y);
	void Scale(Vector2 vec);
	Vector2 Scale() { return scale; }

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	Vector3 Rotation() { return rotation; }

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	Vector3 RotationDegree();

	Vector2 TextureSize() { return textureSize; }
	Vector2 ScaledTextureSize();

	void World(Vector2& S, Vector3& R, Vector2& T);
	Matrix World() { return world; }

	Matrix NormWorld() { return normWorld; }

	void Origin(Matrix mat);
	Matrix Origin() { return origin; }

	void UpdateWorld();
	
	void IsHide(bool val) { isHide = val; }
	bool IsHide() { return isHide; }

private:
	void Initialize(wstring textureFile, wstring shaderFile, float startX, float startY, float endX, float endY);

private:
	struct Vertex
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	Shader* shader;
	UINT pass;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Vector3 direction;
	Matrix D;

	Vector2 position;
	Vector2 scale;
	Vector3 rotation;

	Matrix origin;
	Matrix world;
	Matrix normWorld;

	ID3DX11EffectMatrixVariable* sWorld;
	ID3DX11EffectMatrixVariable* sView;
	ID3DX11EffectMatrixVariable* sProjection;

	Vector2 textureSize;

	wstring textureFile;
	ID3D11ShaderResourceView* srv;

	bool isHide;
};

// ��� �������� �����ϰ� �ʿ���� ����� ���ﶧ
// SRV�� ���� ���� �ٸ� ��ǿ��� ������ ����! (�̹����� ������� ������)
// ���� Ŭ������ �ϳ� ���� �����
// ��������������������������������������������������������������������������������������������������������������������������������������������������������

// SRV�� ���, �����ϴ� Ŭ����
class Sprites
{
private:
	// friend�� ���� private �����ͱ����� ������ �� �� �ִ�.
	// �θ� Ŭ�������� �� ���� ����
	friend class Sprite;

private:
	static ID3D11ShaderResourceView* Load(wstring file);
	static void Remove(wstring file);

private:
	struct SpriteDesc
	{
		// ���� SRV�� ���� ����̸� �� ����� �� ������ RefCount�� ����Ѵ�.
		UINT RefCount = 0;
		ID3D11ShaderResourceView* SRV = NULL;
	};

	static map<wstring, SpriteDesc> spriteMap;
};