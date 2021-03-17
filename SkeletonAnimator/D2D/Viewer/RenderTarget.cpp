#include "stdafx.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
{
	this->width = (width < 1) ? Width : width;
	this->height = (height < 1) ? Height : height;

	// Create Texture (BackBuffer)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = this->width;
		desc.Height = this->height;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;			// 쉐이더는 사이즈를 2^MipLevels의 승수로 만들어준다.		// 거리가 멀어지면 자동으로 줄인다.
		desc.SampleDesc.Count = 1;
		
		HRESULT hr = Device->CreateTexture2D(&desc, NULL, &backBuffer);
		assert(SUCCEEDED(hr));
	}

	// Create RenderTarget (rtv)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		HRESULT hr = Device->CreateRenderTargetView(backBuffer, &desc, &rtv);
		assert(SUCCEEDED(hr));
	}

	// Create SRV (srv)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = Device->CreateShaderResourceView(backBuffer, &desc, &srv);
		assert(SUCCEEDED(hr));
	}
}

RenderTarget::~RenderTarget()
{
	SafeDelete(backBuffer);
	SafeDelete(rtv);
	SafeDelete(srv);
}

void RenderTarget::PreRender()
{
	DeviceContext->OMSetRenderTargets(1, &rtv, NULL);
	DeviceContext->ClearRenderTargetView(rtv, Color(0, 0, 0, 0));
}

void RenderTarget::SetMainTarget()
{
	DeviceContext->OMSetRenderTargets(1, &RTV, NULL);
	DeviceContext->ClearRenderTargetView(RTV, Color(0, 0, 0, 1));
}
