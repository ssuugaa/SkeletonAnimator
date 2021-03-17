#include "stdafx.h"
#include "Line.h"

Line::Line(Vector2 a, Vector2 b)
	: position(0, 0)
	, scale(1, 1)
	, rotation(0, 0, 0)
	, left(a)
	, right(b)
	, pass(0)
{
	CreateBuffer(a, b);

	sWorld = shader->AsMatrix("World");
	sView = shader->AsMatrix("View");
	sProjection = shader->AsMatrix("Projection");

	if (a.x > b.x)
	{
		left = b;
		right = a;
	}

	UpdateWorld();
}

Line::~Line()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);

}

void Line::Update(Matrix & V, Matrix & P)
{
	// V, P
	sView->SetMatrix(V);
	sProjection->SetMatrix(P);

	// Update SubResource
	vertices[0].Position = Vector3(left.x, left.y, 0.0f);
	vertices[1].Position = Vector3(right.x, right.y, 0.0f);

	D3D11_MAPPED_SUBRESOURCE subResource;
	DeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(Vertex) * 2);
	}
	DeviceContext->Unmap(vertexBuffer, 0);
}

void Line::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	shader->Draw(0, pass, 2);
}

void Line::Position(float x, float y)
{
	Position(Vector2(x, y));
}

void Line::Position(Vector2 & vec)
{
	position = vec;
	UpdateWorld();
}

void Line::AddPosition(Vector2 & vec)
{
	position += vec;
	UpdateWorld();
}

void Line::Scale(float x, float y)
{
	Scale(Vector2(x, y));
}

void Line::Scale(Vector2 & vec)
{
	scale = vec;
	UpdateWorld();
}

void Line::Rotation(float z)
{
	Rotation(Vector3(0, 0, z));
}

void Line::Rotation(Vector3 & vec)
{
	rotation = vec;
	UpdateWorld();
}

void Line::CreateBuffer(Vector2 a, Vector2 b)
{
	shader = new Shader(L"41_Line.fx");

	vertices[0].Position = Vector3(a.x, a.y, 0.0f);
	vertices[1].Position = Vector3(b.x, b.y, 0.0f);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = sizeof(Vertex) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertices;

	HRESULT hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));
}

void Line::UpdateWorld()
{
	Matrix W, S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, 1.0f);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.x, rotation.y, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, 0.0f);
	W = S * R * T;

	sWorld->SetMatrix(W);
}
