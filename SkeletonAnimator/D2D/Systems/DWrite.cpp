#include "stdafx.h"
#include "DWrite.h"

DWrite* DWrite::instance = NULL;

IDWriteFactory* DWrite::writeFactory = NULL;		
ID2D1DeviceContext* DWrite::deviceContext = NULL;	
ID2D1Bitmap1* DWrite::bitmap = NULL;
IDXGISurface* DWrite::surface = NULL;
ID2D1SolidColorBrush* DWrite::brush = NULL;
IDWriteTextFormat* DWrite::format = NULL;

DWrite::DWrite()
{
	HRESULT hr;
	hr = DWriteCreateFactory
	(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)&writeFactory
	);
	assert(SUCCEEDED(hr));

	D2D1_FACTORY_OPTIONS option;
	option.debugLevel = D2D1_DEBUG_LEVEL_WARNING;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	assert(SUCCEEDED(hr));

	IDXGIDevice* dxgiDevice;
	hr = Device->QueryInterface(&dxgiDevice);
	assert(SUCCEEDED(hr));
	
	hr = factory->CreateDevice(dxgiDevice, &device);
	assert(SUCCEEDED(hr));

	hr = device->CreateDeviceContext
	(
		D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&deviceContext
	);
	assert(SUCCEEDED(hr));

	CreateBackBuffer();
}

DWrite::~DWrite()
{
	// static ������ static�Լ������� ���� ����??
	DeleteBackBuffer();

	SafeRelease(device);
	SafeRelease(deviceContext);

	SafeRelease(factory);
	SafeRelease(writeFactory);
}

void DWrite::Create()
{
	assert(instance == NULL);

	instance = new DWrite();
}

void DWrite::Delete()
{
	SafeDelete(instance);
}

DWrite * DWrite::Get()
{
	return instance;
}

void DWrite::CreateBackBuffer()
{
	HRESULT hr;

	hr = SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&surface);
	assert(SUCCEEDED(hr));

	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;	// ��Ʈ �Է½� ����� ����
	bp.dpiX = 96;
	bp.dpiY = 96;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = NULL;
	
	hr = deviceContext->CreateBitmapFromDxgiSurface(surface, &bp, &bitmap);
	assert(SUCCEEDED(hr));

	deviceContext->SetTarget(bitmap);

	hr = deviceContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &brush);	// ��Ʈ ���� ����
	assert(SUCCEEDED(hr));

	writeFactory->CreateTextFormat
	(
		L"����ü",	// �۾�ü ����
		NULL,		// �ܺ� �ٿ�ε� ��Ʈ?
		DWRITE_FONT_WEIGHT_SEMI_BOLD,	// ��Ʈ ����
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15,			// ��Ʈ ������
		L"ko",		// ���� ����
		&format
	);
}

// static ������ static�Լ������� ���� ����??
void DWrite::DeleteBackBuffer()
{
	deviceContext->SetTarget(NULL);

	SafeRelease(brush);
	SafeRelease(format);

	SafeRelease(bitmap);
	SafeRelease(surface);
}



void DWrite::RenderText(wstring & text, RECT & rect)
{
	D2D1_RECT_F rt = 
	{
		(FLOAT)rect.left,
		(FLOAT)rect.top,
		(FLOAT)rect.right,
		(FLOAT)rect.bottom
	};
	
	deviceContext->DrawTextW(text.c_str(), text.length(), format, &rt, brush);
}