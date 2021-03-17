#pragma once

// ΩÃ±€≈Ê
class DWrite
{
private:
	DWrite();
	~DWrite();

public:
	static void Create();
	static void Delete();

	static DWrite* Get();
	static ID2D1DeviceContext* GetDC() { return deviceContext; }

	static void CreateBackBuffer();
	static void DeleteBackBuffer();

	static void RenderText(wstring& text, RECT& rect);

private:
	static DWrite* instance;

	// D2D 
	ID2D1Factory1* factory;						// D2D Device
	static IDWriteFactory* writeFactory;		// D2D Font Desc

	ID2D1Device* device;
	static ID2D1DeviceContext* deviceContext;	// DC

	static ID2D1Bitmap1* bitmap;				// RTV
	static IDXGISurface* surface;				// BackBuffer

	static ID2D1SolidColorBrush* brush;			// Font Color
	static IDWriteTextFormat* format;			// Font Style
};