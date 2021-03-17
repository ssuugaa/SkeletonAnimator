#pragma once

//Common Header
#include <Windows.h>
#include <assert.h>

//STL
#include <vector>
#include <string>
#include <bitset>	// bool 타입만 다루는 배열
#include <map>
#include <functional>
#include <mutex>
using namespace std;

//DirectX Libraries
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3DX10math.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "Effects11d.lib")
#pragma comment(lib, "d3dcompiler.lib")

// ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "ImGui.lib")

// DWrite
#include <d2d1_2.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "fmod/fmod.hpp"
#pragma comment(lib, "fmod/fmodex_vc.lib")

// typedef
typedef D3DXVECTOR2	Vector2;
typedef D3DXVECTOR3	Vector3;
typedef D3DXMATRIX	Matrix;
typedef D3DXCOLOR	Color;

// FrameWork
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/DWrite.h"

#include "Renders/Shader.h"
#include "Renders/Sprite.h"

#include "Utilities/String.h"
#include "Utilities/Math.h"

#include "Viewer/Camera.h"
#include "Viewer/Freedom.h"

// Macro
#define SafeDelete(p) {if(p) { delete (p); (p) = NULL;}}
#define SafeDeleteArray(p) {if(p) { delete[] (p); (p) = NULL;}}
#define SafeRelease(p) {if(p) { (p)->Release(); (p) = NULL;}}

#define GENERATE_DEMO(out) \
Matrix V = values->MainCamera->View(); \
Matrix P = values->Projection; \
\
out = Mouse->Position(); \
out.x = (out.x / Width) * 2.0f - 1.0f; \
out.y = ((out.y / Height) * 2.0f - 1.0f) * -1.0f; \
Matrix vp = V * P; \
D3DXMatrixInverse(&vp, NULL, &vp); \
D3DXVec2TransformCoord(&out, &out, &vp);

//Global
extern UINT Width;
extern UINT Height;

extern HWND Hwnd;
extern wstring Title;

//DirectX Interfaces
extern IDXGISwapChain* SwapChain;
extern ID3D11Device* Device;
extern ID3D11DeviceContext* DeviceContext;
extern ID3D11RenderTargetView* RTV;

extern Keyboard* Key;
extern C_Mouse* Mouse;
