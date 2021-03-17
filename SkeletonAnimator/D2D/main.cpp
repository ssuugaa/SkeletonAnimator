#include "stdafx.h"
#include "Systems/Device.h"
#include "Scenes/Scene.h"

#include "Scenes/S00_AnimCustom.h"

SceneValues* values = NULL;	// ��, ��������
vector<Scene*> scenes;	// ���� ����ϴ� ����

void InitScene()
{
	values = new SceneValues();
	values->MainCamera = new Freedom();		// ����Ʈ ī�޶� - �� ���ο��� �����Ѵ�
	D3DXMatrixIdentity(&values->Projection);

	scenes.push_back(new S00_AnimCustom(values));
}

void DestroyScene()
{
	SafeDelete(values);
	for (Scene* scene : scenes) SafeDelete(scene);
}

void Update()
{
	// View, Projection
	values->MainCamera->Update();
	D3DXMatrixOrthoOffCenterLH(&values->Projection, (float)Width * -0.5f, (float)Width * 0.5f, (float)Height * -0.5f, (float)Height * 0.5f, -1, 1);

	for (Scene* scene : scenes)
		scene->Update();
}

void Render()
{
	DeviceContext->ClearRenderTargetView(RTV, Color(0.3f, 0.3f, 0.3f, 1));
	{
		for (Scene* scene : scenes)
			scene->Render();
	}
	ImGui::Render();

	//DWrite::GetDC()->BeginDraw();
	//{
	//	RECT rect = {0, 0, 500, 200};
	//	wstring text = L"fps : " + to_wstring(ImGui::GetIO().Framerate);
	//	DWrite::RenderText(text, rect);
	//
	//	rect.top += 20;
	//	rect.bottom += 20;
	//	text = L"fps : " + to_wstring(Time::Get()->FPS());
	//	DWrite::RenderText(text, rect);
	//}
	//DWrite::GetDC()->EndDraw();
	
	// (1, 0)�� �ٲٸ� 60���������� ������ �� �ִ�!
	// (2, 0) = 30������, (3, 0) = 20������...
	SwapChain->Present(3, 0);
}
