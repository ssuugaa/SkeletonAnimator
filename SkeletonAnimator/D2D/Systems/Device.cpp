#include "stdafx.h"
#include "Device.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	InitWindow(hInstance, nCmdShow);
	InitDirect3D(hInstance);

	Running();
	Destroy();

	return 0;
}

UINT Width = 1920;
UINT Height = 960;

HWND Hwnd = NULL;
wstring Title = L"D2D";

IDXGISwapChain* SwapChain = NULL;
ID3D11Device* Device = NULL;
ID3D11DeviceContext* DeviceContext = NULL;
ID3D11RenderTargetView* RTV = NULL;

Keyboard* Key = NULL;
C_Mouse* Mouse = NULL;

void InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//Register WndClass
	{
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)WHITE_BRUSH;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = Title.c_str();
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		WORD check = RegisterClassEx(&wc);
		assert(check != NULL);
	}

	//Create HWND
	{
		Hwnd = CreateWindowEx
		(
			NULL,
			Title.c_str(),
			Title.c_str(),
			WS_EX_TOPMOST | WS_POPUP, // | WS_OVERLAPPEDWINDOW,
			0,
			0,
			Width,
			Height,
			GetDesktopWindow(),
			NULL,
			hInstance,
			NULL
		);
		assert(Hwnd != NULL);
	}

	RECT rect = { 0, 0, (LONG)Width, (LONG)Height };
	// 실제화면 크기를 가져온다 - 화면 중앙좌표를 얻는다.
	int CenterX = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
	int CenterY = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;
	// 이동한 화면 좌표로 적용
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	// 화면을 중앙으로 이동.
	//MoveWindow(Hwnd, CenterX, CenterY - 30, Width, Height, TRUE);
	SetWindowPos(Hwnd, NULL, CenterX, CenterY - 30, Width, Height, SWP_NOSIZE);

	ShowWindow(Hwnd, nCmdShow);
	UpdateWindow(Hwnd);
}

void InitDirect3D(HINSTANCE hInstance)
{
	//SwapChain.Buffer
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//SwapChain
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferDesc = bufferDesc;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.OutputWindow = Hwnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//기능 수준 정의
	vector<D3D_FEATURE_LEVEL> feature_level = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	//Create (1)Device & (2)SwapChain & (3)DeviceContext
	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		feature_level.data(),
		feature_level.size(),
		D3D11_SDK_VERSION,
		&swapDesc,
		&SwapChain,
		&Device,
		NULL,
		&DeviceContext
	);
	assert(SUCCEEDED(hr));

	CreateBackBuffer();
}

void Destroy()
{
	SafeDelete(Mouse);
	SafeDelete(Key);

	DeleteBackBuffer();
	SafeRelease(DeviceContext);
	SafeRelease(Device);
	SafeRelease(SwapChain);
}

WPARAM Running()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	ImGui::Create(Hwnd, Device, DeviceContext);
	ImGui::StyleColorsDark();

	DWrite::Create();

	Time::Create();
	Time::Get()->Start();

	Key = new Keyboard();
	Mouse = new C_Mouse(Hwnd);

	InitScene();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Time::Get()->Update();
			Mouse->Update();
			Update();
			ImGui::Update();
			Render();
		}
	}

	DestroyScene();

	SafeDelete(Mouse);
	SafeDelete(Key);

	Time::Delete();
	DWrite::Delete();
	ImGui::Delete();

	return msg.message;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::WndProc((void*)Hwnd, msg, wParam, lParam))
		return true;

	if (Mouse != NULL)
		Mouse->WndProc(msg, wParam, lParam);

	switch (msg)
	{
		
		case WM_SIZE:
		{
			if (Device != NULL)
			{
				ImGui::Invalidate();

				Width = LOWORD(lParam);
				Height = HIWORD(lParam);
				/*
				wstring temp = to_wstring(Width) + L", " + to_wstring(Height);
				MessageBox(hwnd, L"", temp.c_str(), MB_OK);
				*/
				DeleteBackBuffer();
				DWrite::DeleteBackBuffer();

				HRESULT hr = SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
				assert(SUCCEEDED(hr));

				DWrite::CreateBackBuffer();
				CreateBackBuffer();

				ImGui::Validate();
			}
		}
		break;

		case WM_DESTROY: PostQuitMessage(0); break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateBackBuffer() 
{
	//BackBuffer
	ID3D11Texture2D* BackBuffer;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	assert(SUCCEEDED(hr));

	//Create RTV (랜더 타겟 뷰)
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, &RTV);
	assert(SUCCEEDED(hr));
	SafeRelease(BackBuffer);

	//OMSet
	DeviceContext->OMSetRenderTargets(1, &RTV, NULL);

	//Create Viewport
	{
		D3D11_VIEWPORT viewPort;
		ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = (float)Width;
		viewPort.Height = (float)Height;

		DeviceContext->RSSetViewports(1, &viewPort);
	}
}

void DeleteBackBuffer() 
{
	SafeRelease(RTV);
}