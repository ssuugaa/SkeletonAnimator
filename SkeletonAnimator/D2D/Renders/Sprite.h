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

// 모션 여러개를 생성하고 필요없는 모션을 지울때
// SRV도 같이 비우면 다른 모션에서 난리가 난다! (이미지가 사라졌기 때문에)
// 따라서 클래스를 하나 따로 만든다
// ────────────────────────────────────────────────────────────────────────────

// SRV를 담당, 관리하는 클래스
class Sprites
{
private:
	// friend는 나의 private 데이터까지도 가져다 쓸 수 있다.
	// 부모 클래스보다 더 강한 공유
	friend class Sprite;

private:
	static ID3D11ShaderResourceView* Load(wstring file);
	static void Remove(wstring file);

private:
	struct SpriteDesc
	{
		// 같은 SRV를 쓰는 모션이면 그 모션의 총 갯수를 RefCount에 등록한다.
		UINT RefCount = 0;
		ID3D11ShaderResourceView* SRV = NULL;
	};

	static map<wstring, SpriteDesc> spriteMap;
};