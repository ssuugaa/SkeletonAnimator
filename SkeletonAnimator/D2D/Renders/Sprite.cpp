#include "stdafx.h"
#include "Sprite.h"

// ────────────────────────────────────────────────────────────────────────────
// Sprite
// ────────────────────────────────────────────────────────────────────────────

// 한 개 있는 경우
Sprite::Sprite(wstring textureFile, wstring shaderFile)
{
	Initialize(textureFile, shaderFile, 0, 0, 0, 0);
}

// 한 줄, 여러 개 있는 경우
Sprite::Sprite(wstring textureFile, wstring shaderFile, float endX, float endY)
{
	Initialize(textureFile, shaderFile, 0, 0, endX, endY);
}

// 여러 줄, 여러 개 있는 경우
Sprite::Sprite(wstring textureFile, wstring shaderFile, float startX, float startY, float endX, float endY)
{
	Initialize(textureFile, shaderFile, startX, startY, endX, endY);
}

Sprite::Sprite(wstring textureFile, wstring shaderFile, Vector2 position, Vector2 scale)
{
	Initialize(textureFile, shaderFile, 0, 0, 0, 0);
	
	this->position = position;
	this->scale = scale;
	UpdateWorld();
}

// 사실상 생성자 역할
void Sprite::Initialize(wstring textureFile, wstring shaderFile, float startX, float startY, float endX, float endY)
{
	this->textureFile = L"../../Image/" + textureFile;
	
	shader = new Shader(shaderFile);
	pass = 0;

	// srv 쉐이더에 넘기기
	srv = Sprites::Load(this->textureFile);
	shader->AsShaderResource("DiffuseMap")->SetResource(srv);

	D3DX11_IMAGE_INFO info;
	HRESULT hr = D3DX11GetImageInfoFromFile(this->textureFile.c_str(), NULL, &info, NULL);
	assert(SUCCEEDED(hr));

	// uv 좌표값 계산
	//startX = (startX > 0) ? startX / (float)info.Width : 0.0f;
	//startY = (startY > 0) ? startY / (float)info.Height : 0.0f;

	//endX = (endX > 0) ? (endX + 0.0001f) / (float)info.Width : ((float)info.Width + 0.0001f) / (float)info.Width;
	//endY = (endY > 0) ? (endY + 0.0001f) / (float)info.Height : ((float)info.Height + 0.0001f) / (float)info.Height;

	startX = (startX > 0) ? startX / (float)info.Width + 1e-6f : 1e-6f;
	startY = (startY > 0) ? startY / (float)info.Height + 1e-6f : 1e-6f;

	endX = (endX > 0) ? endX / (float)info.Width : 1;
	endY = (endY > 0) ? endY / (float)info.Height : 1;

	// 그림 그릴 사각형 생성
	Vertex vertices[4];

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[0].Uv = Vector2(startX, endY);
	vertices[1].Uv = Vector2(startX, startY);
	vertices[2].Uv = Vector2(endX, endY);
	vertices[3].Uv = Vector2(endX, startY);

	float sizeX = (endX > 0) ? endX * (float)info.Width : (float)info.Width;
	sizeX -= startX * (float)info.Width;

	float sizeY = (endY > 0) ? endY * (float)info.Height : (float)info.Height;
	sizeY -= startY * (float)info.Height;

	textureSize = Vector2(sizeX, sizeY);

	// Default World
	direction = Vector3(0, 0, 0);
	D3DXMatrixRotationYawPitchRoll(&D, direction.x, direction.y, direction.z);

	scale = Vector2(1, 1);
	position = Vector2(0, 0);
	rotation = Vector3(0, 0, 0);
	D3DXMatrixIdentity(&origin);
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&normWorld);


	sWorld = shader->AsMatrix("World");
	sView = shader->AsMatrix("View");
	sProjection = shader->AsMatrix("Projection");
	sAlpha = shader->AsScalar("Alpha");
	sAlpha->SetFloat(1.0f);

	UpdateWorld();

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// 포인터가 멤버변수일 경우엔 { 0 } 초기화하면 에러날 수 있다
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create IndexBuffer
	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		// 버퍼의 크기 중요!
		desc.ByteWidth = sizeof(UINT) * 6;
		// 인덱스로 쓸거다!
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;

		HRESULT hr = Device->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}


Sprite::~Sprite()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	Sprites::Remove(textureFile);
}



void Sprite::Update(Matrix & V, Matrix & P)
{
	sView->SetMatrix(V);
	sProjection->SetMatrix(P);
}

void Sprite::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	shader->DrawIndexed(0, pass, 6);
}

void Sprite::Position(float x, float y)
{
	Position(Vector2(x, y));
}
void Sprite::Position(Vector2 vec)
{
	position = vec;

	UpdateWorld();
}
void Sprite::AddPosition(Vector2 vec)
{
	position += vec;
	UpdateWorld();
}
void Sprite::Scale(float x, float y)
{
	Scale(Vector2(x, y));
}
void Sprite::Scale(Vector2 vec)
{
	scale = vec;

	UpdateWorld();
}
void Sprite::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}
void Sprite::Rotation(Vector3 & vec)
{
	rotation = vec;

	UpdateWorld();
}
void Sprite::RotationDegree(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}
void Sprite::RotationDegree(Vector3 & vec)
{
	vec.x = Math::ToRadian(vec.x);
	vec.y = Math::ToRadian(vec.y);
	vec.z = Math::ToRadian(vec.z);
	
	Rotation(vec);
}
Vector3 Sprite::RotationDegree()
{
	Vector3 vec;
	vec.x = Math::ToDegree(rotation.x);
	vec.y = Math::ToDegree(rotation.y);
	vec.z = Math::ToDegree(rotation.z);
	return vec;
}

Vector2 Sprite::ScaledTextureSize()
{
	return Vector2(textureSize.x * scale.x, textureSize.y * scale.y);
}

void Sprite::World(Vector2 & S, Vector3 & R, Vector2 & T)
{
	scale		= S;
	rotation	= R;
	position	= T;

	UpdateWorld();
}

void Sprite::Origin(Matrix mat)
{
	origin = mat;

	UpdateWorld();
}

void Sprite::UpdateWorld()
{
	Matrix ID, TS, nS, R, T;

	D3DXMatrixScaling(&TS, textureSize.x, textureSize.y, 1.0f);
	D3DXMatrixScaling(&nS, scale.x, scale.y, 1.0f);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, 0.0f);

	D3DXMatrixInverse(&ID, NULL, &D);

	world = TS * D * nS * ID * R * T * origin;
	normWorld = D * nS * ID * R * T * origin;

	sWorld->SetMatrix(world);
}

void Sprite::Alpha(float alpha)
{
	sAlpha->SetFloat(alpha);
}




// ────────────────────────────────────────────────────────────────────────────
// Sprites(SRV 관리 클래스)
// ────────────────────────────────────────────────────────────────────────────

// 스태틱 선언
map<wstring, Sprites::SpriteDesc> Sprites::spriteMap;

ID3D11ShaderResourceView * Sprites::Load(wstring file)
{
	// 맵에 등록되어 있으면 RefCount에 1 더하고 SRV값을 리턴
	if (spriteMap.count(file) > 0)
	{
		spriteMap[file].RefCount++;

		return spriteMap[file].SRV;
	}

	// 맵에 등록되어있지 않으면 만들어서 추가
	// 최초의 RefCount값이 1이다
	ID3D11ShaderResourceView* srv;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		Device, file.c_str(), NULL, NULL, &srv, NULL
	);
	assert(SUCCEEDED(hr));

	SpriteDesc desc;
	desc.RefCount++;
	desc.SRV = srv;

	spriteMap[file] = desc;

	return srv;
}

void Sprites::Remove(wstring file)
{
	// 호출 시 카운트 감소
	UINT count = spriteMap.count(file);
	assert(count > 0);

	spriteMap[file].RefCount--;

	// srv를 쓰는 곳이 없으면 srv제거
	if (spriteMap[file].RefCount < 1)
	{
		SafeRelease(spriteMap[file].SRV);
		spriteMap.erase(file);
	}
}