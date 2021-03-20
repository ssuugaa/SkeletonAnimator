#include "stdafx.h"
#include "S00_AnimCustom.h"
#include "Objects/Line.h"

const UINT FixWidth = Width;
const UINT FixHeight = Height;

S00_AnimCustom::S00_AnimCustom(SceneValues * values)
	: Scene(values)
	, fileName("HumanKing01")		// 편집 타겟!
	, file(NULL)
	, phase(0)
	, partNum(0)
	, partSelect(0)
	, animSelect(0)
	, dataSelect(NULL)
	, scroll(1)
	, timePlay(0)
	, isPlay(false)
{
	// fixed Cam
	D3DXMatrixIdentity(&fixedView);
	D3DXVECTOR3 eye = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 at = D3DXVECTOR3(0, 0, 1);
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0);
	D3DXMatrixLookAtLH(&fixedView, &eye, &(eye + at), &up);
	D3DXMatrixOrthoOffCenterLH(&fixedProj, (float)Width * -0.5f, (float)Width * 0.5f, (float)Height * -0.5f, (float)Height * 0.5f, -1, 1);

	// 마우스
	mouseLog = Mouse->Position();
	
	// ImGui
	phaseMenu[0] = "Character Init";
	phaseMenu[1] = "Animation";

	// UI
	shader = L"14_Sprite.fx";
	fileS = L"../SkeletonAnimator/UI/Scale.png";
	fileR = L"../SkeletonAnimator/UI/Rotate.png";
	fileT = L"../SkeletonAnimator/UI/Position.png";
	
	tableBackground = new Sprite(L"../SkeletonAnimator/UI/tableBackground.png", shader);
	tableBackground->Position(0,  150 - Height * 0.5f);
	tableBackground->Scale(1.92f, 1.5f);
	
	centerLine.push_back(new Line(Vector2(-1000, 0), Vector2(1000, 0)));
	centerLine.push_back(new Line(Vector2(0, -1000), Vector2(0, 1000)));

	direction = new Line(Vector2(0, -2000), Vector2(0, 2000));

	for (int i = 0; i < 2; i++)
		timeHLines.push_back(new Line(Vector2(-1000, 300 - i * 20 - Height * 0.5f), Vector2(1000, 300 - i * 20 - Height * 0.5f)));

	for (int i = 2; i < 2 + 3; i++)
		timeHLines.push_back(new Line(Vector2(-1000, 300 - i * 30 - Height * 0.5f), Vector2(1000, 300 - i * 30 - Height * 0.5f)));

	for (int i = 0; i <= 90; i++)
	{
		timeVLines.push_back(new Line(Vector2(110 + 20 * i - Width * 0.5f, 300 - Height * 0.5f), Vector2(110 + 20 * i - Width * 0.5f, 300 - 30 - Height * 0.5f)));
		if (i % 10 == 0) timeVLines[i]->Pass(1);
	}
	
	timeLineMax = new Line(Vector2(0 - Width * 0.5f, 300 - Height * 0.5f), Vector2(0 - Width * 0.5f, 0 - Height * 0.5f));
	timeLineMax->Pass(2);
	timeLineSelect = new Line(Vector2(0 - Width * 0.5f, 300 - Height * 0.5f), Vector2(0 - Width * 0.5f, 0 - Height * 0.5f));
	timeLineSelect->Position(Vector2(110, 0));

	// 임시
	//int Xp = -330, Yp = 160;
	//TempLines.push_back(new Line(Vector2(Xp - 106 * 4, Yp), Vector2(Xp + 106 * 4, Yp)));
	//TempLines.push_back(new Line(Vector2(Xp - 110 * 4, Yp - 45), Vector2(Xp + 110 * 4, Yp - 45)));
	//TempLines.push_back(new Line(Vector2(Xp - 115 * 4, Yp - 45 - 52), Vector2(Xp + 115 * 4, Yp - 45 - 52)));
	//TempLines.push_back(new Line(Vector2(Xp - 121 * 4, Yp - 45 - 52 - 60), Vector2(Xp + 121 * 4, Yp - 45 - 52 - 60)));
	//TempLines.push_back(new Line(Vector2(Xp - 128 * 4, Yp - 45 - 52 - 60 - 69), Vector2(Xp + 128 * 4, Yp - 45 - 52 - 60 - 69)));
	//
	//TempLines.push_back(new Line(Vector2(Xp - 136 * 4, Yp - 612 + 115 + 102 + 90), Vector2(Xp + 136 * 4, Yp - 612 + 115 + 102 + 90)));
	//TempLines.push_back(new Line(Vector2(Xp - 145 * 4, Yp - 612 + 115 + 102), Vector2(Xp + 145 * 4, Yp - 612 + 115 + 102)));
	//TempLines.push_back(new Line(Vector2(Xp - 155 * 4, Yp - 612 + 115), Vector2(Xp + 155 * 4, Yp - 612 + 115)));
	//TempLines.push_back(new Line(Vector2(Xp - 166 * 4, Yp - 612), Vector2(Xp + 166 * 4, Yp - 612)));
	//
	//for (int i = 0; i < 9; i++)
	//{
	//	TempLines.push_back(new Line(Vector2(Xp - 106 * (4 - i), Yp), Vector2(Xp - 166 * (4 - i), Yp - 612)));
	//}

	// 데이터 로드
	ZeroMemory(buf, sizeof(buf));
	LoadData(fileName);
	LoadAnim(fileName);

	// 1회 업데이트
	direction->Position(parts[0]->World()._41, parts[0]->World()._42);
	direction->Rotation(-partDatas[0]->D);
	UpdateWorld(partDatas[0], parts[0]);

	tableBackground->Update(fixedView, fixedProj);
	
	for (UINT i = 0; i < timeHLines.size(); i++)
		timeHLines[i]->Update(fixedView, fixedProj);
	for (UINT i = 0; i < timeVLines.size(); i++)
		timeVLines[i]->Update(fixedView, fixedProj);

	timeLineSelect->Update(fixedView, fixedProj);
	timeLineMax->Update(fixedView, fixedProj);

	for (int i = 0; i < animNum; i++)
	{
		for (UINT j = 0; j < anims[i]->flagS.size(); j++)
			anims[i]->flagS[j].second->Update(fixedView, fixedProj);
		for (UINT j = 0; j < anims[i]->flagR.size(); j++)
			anims[i]->flagR[j].second->Update(fixedView, fixedProj);
		for (UINT j = 0; j < anims[i]->flagT.size(); j++)
			anims[i]->flagT[j].second->Update(fixedView, fixedProj);
	}
}

S00_AnimCustom::~S00_AnimCustom()
{
	SafeDelete(file);
	SafeDelete(dataSelect);
	SafeDelete(tableBackground);
	SafeDelete(timeLineMax);
	SafeDelete(timeLineSelect);
	SafeDelete(direction);
	SafeDelete(phaseMenu[1]); SafeDelete(phaseMenu[0]);
	SafeDelete(centerLine[1]); SafeDelete(centerLine[0]);

	for (int i = 0; i < partNum; i++)
	{
		SafeDelete(parts[i]);
		SafeDelete(partDatas[i]);
	}
	for (int i = 0; i < animNum; i++)
		SafeDelete(anims[i]);
	for (UINT i = 0; i < timeHLines.size(); i++)
		SafeDelete(timeHLines[i]);
	for (UINT i = 0; i < timeVLines.size(); i++)
		SafeDelete(timeVLines[i]);

	// 임시
	//for (UINT i = 0; i < TempLines.size(); i++)
	//	SafeDelete(TempLines[i]);

	parts.clear();
	partDatas.clear();
	anims.clear();
	centerLine.clear();
	timeHLines.clear();
	timeVLines.clear();
}

void S00_AnimCustom::Update()
{
	D3DXMatrixOrthoOffCenterLH(&values->Projection, (float)FixWidth * scroll * -0.5f, (float)FixWidth * scroll * 0.5f, (float)Height * scroll * -0.5f, (float)Height * scroll * 0.5f, -1, 1);

	Vector2 mouse;
	GENERATE_DEMO(mouse);	// V, P 생성 & 마우스 현지화

	// 마우스 입력
	InputState();
	switch (state)
	{
		case STATE::setDirection	: SetDirection();	break;
		case STATE::move			: Move();			break;
		case STATE::stretch			: Stretch();		break;
		case STATE::rotate			: Rotate();			break;
		case STATE::timeShift		: TimeShift();		break;
		case STATE::timeMaxShift	: TimeMaxShift();	break;
		case STATE::deleteFlag		: DeleteFlag();		break;
		case STATE::reset			: Reset();			break;
	}

	// 재생
	Play();

	// Time Max 위치
	if(anims.size() != 0 && phase == 1)
		timeLineMax->Position(Vector2(anims[animSelect]->timeMax / 4.5f * 1800 + 110, 0));
	
	// 확대 / 축소
	if (Mouse->Wheel() > 0)
	{
		if (scroll > 0.01f) scroll *= 0.7f;
	}
	else if (Mouse->Wheel() < 0) scroll *= 1.3f;


	if (Key->Down(VK_RETURN))		// 엔터
	{
		parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);
	}

	// Update
	for (Sprite* part : parts)
		part->Update(V, P);

	if(partDatas[1]->parent != -1)

	for (int i = 0; i < 2; i++)
		centerLine[i]->Update(V, P);
	
	direction->Update(V, P);

	// fixed Cam - 화면 따라 움직이면 안되는 애들
	if (phase == 1)
	{
	}

	// 마우스 위치 갱신
	mouseLog = Mouse->Position();

	// 임시
	//for (UINT i = 0; i < TempLines.size(); i++)
	//	TempLines[i]->Update(V, P);
}

void S00_AnimCustom::InputState()
{
	// 임구이 누른거면 탈락
	if (ImGui::GetIO().WantCaptureMouse) return;
	// 마우스를 누르고 있는 중이 아니라면 아무것도 안함
	if (!Mouse->Press(0))
	{
		state = STATE::none;
		if (Key->Press(VK_CONTROL) && Key->Down('s'))
		{
			if(phase == 0) SaveData(fileName);
			if(phase == 1) SaveAnim(fileName);
		}
		if (Key->Down(VK_SPACE))	// 위치 보정
		{
			dataSelect->T = Vector2(floor(dataSelect->T.x), floor(dataSelect->T.y));
 			if((int)parts[partSelect]->TextureSize().x % 2 == 0) dataSelect->T.x += 0.5f;
			if((int)parts[partSelect]->TextureSize().y % 2 == 0) dataSelect->T.y += 0.5f;
			parts[partSelect]->Position(dataSelect->T);
			UpdateWorld(partDatas[partSelect], parts[partSelect]);
			direction->Position(parts[partSelect]->World()._41, parts[partSelect]->World()._42);
		}


		return;
	}
	// 행동중인 경우 탈락
	if (state != STATE::none) return;



	// 테이블 내에 있으면 시간 조절
	if (state == STATE::none)
	{
		// 타임테이블 영역
		if (phase == 1 && Mouse->Position().y > Height - 300)
		{
			if (Key->Press(VK_TAB))
			{
				state = STATE::timeMaxShift;
				return;
			}
			if (Key->Press(VK_CONTROL))
			{
				state = STATE::deleteFlag;
				return;
			}
			if (Key->Press(VK_SHIFT))
			{
				state = STATE::reset;
				return;
			}
			state = STATE::timeShift;
			return;
		}

		// 이미지 영역
		if (Key->Press(VK_MENU))		// Alt
		{
			state = STATE::setDirection;
			return;
		}
		if (Key->Press(VK_SHIFT))
		{
			state = STATE::rotate;
			return;
		}
		if (Key->Press(VK_CONTROL))
		{
			state = STATE::stretch;
			return;
		}
		state = STATE::move;
		return;
	}
}

void S00_AnimCustom::UpdateWorld(DataSet * set, Sprite * sprite)
{
	int count = set->child.size();
	for (int i = 0; i < count; i++)
	{
		parts[set->child[i]]->Origin(sprite->NormWorld());
		UpdateWorld(partDatas[set->child[i]], parts[set->child[i]]);
	}
}
void S00_AnimCustom::UpdateSprite()
{
	// SRT
	int s, r, t, h;
	Vector2 vs = { 1, 1 };
	Vector3 vr = { 0, 0, 0 };
	Vector2 vt = { 0, 0 };

	for (UINT p = 0; p < partDatas.size(); p++)
	{
		s = 0, r = 0, t = 0, h = -1;

		// T 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataT.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataT[i].first)
				t++;
		if (t == anims[animSelect]->parts[p]->dataT.size())
		{
			vt = anims[animSelect]->parts[p]->dataT[t - 1].second;
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataT[t].first
			&& anims[animSelect]->parts[p]->dataT[t].first < timeSelect + 1e-3f)
		{
			vt = anims[animSelect]->parts[p]->dataT[t].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataT[t].first - anims[animSelect]->parts[p]->dataT[t - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[p]->dataT[t - 1].first) / delta;

			vt = anims[animSelect]->parts[p]->dataT[t - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataT[t].second * delta;
		}

		// R 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataR.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataR[i].first)
				r++;
		if (r == anims[animSelect]->parts[p]->dataR.size())
		{
			vr = anims[animSelect]->parts[p]->dataR[r - 1].second;
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataR[r].first
			&& anims[animSelect]->parts[p]->dataR[r].first < timeSelect + 1e-3f)
		{
			vr = anims[animSelect]->parts[p]->dataR[r].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataR[r].first - anims[animSelect]->parts[p]->dataR[r - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[p]->dataR[r - 1].first) / delta;

			vr = anims[animSelect]->parts[p]->dataR[r - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataR[r].second * delta;
		}

		// S 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataS.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataS[i].first)
				s++;
		if (s == anims[animSelect]->parts[p]->dataS.size())
		{
			vs = anims[animSelect]->parts[p]->dataS[s - 1].second;
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataS[s].first
			&& anims[animSelect]->parts[p]->dataS[s].first < timeSelect + 1e-3f)
		{
			vs = anims[animSelect]->parts[p]->dataS[s].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataS[s].first - anims[animSelect]->parts[p]->dataS[s - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[p]->dataS[s - 1].first) / delta;

			vs = anims[animSelect]->parts[p]->dataS[s - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataS[s].second * delta;
		}

		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataH.size(); i++)
			if (timeSelect + 1e-3f > anims[animSelect]->parts[partSelect]->dataH[i].first)
				h++;

		// 적용
		parts[p]->IsHide(anims[animSelect]->parts[p]->dataH[h].second);
		parts[p]->World(vs, vr, vt);
	}
	UpdateWorld(partDatas[0], parts[0]);
}

void S00_AnimCustom::SetDirection()
{
	float temp = Mouse->Position().x - mouseLog.x;
	temp *= 0.01f;
	temp += parts[partSelect]->Direction();

	if (phase == 0)
	{
		partDatas[partSelect]->D = temp;

		parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);
		direction->Rotation(-dataSelect->D);
		parts[partSelect]->Direction(dataSelect->D);
	}
	//else if (animNum > 0)
	//{
	//	bool New = true;
	//	vector<pair<float, Vector3>>::iterator index = anims[animSelect]->parts[partSelect]->dataR.begin();
	//	//int index = 0;
	//	for (int i = 0; i < anims[animSelect]->parts[partSelect]->dataR.size(); i++)
	//	{
	//		if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataR[i].first) index++;
	//
	//		if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataR[i].first
	//			&& anims[animSelect]->parts[partSelect]->dataR[i].first < timeSelect + 1e-3f)
	//		{
	//			anims[animSelect]->parts[partSelect]->dataR[i].second = temp;
	//			New = false;
	//			break;
	//		}
	//	}
	//	if (New)
	//	{
	//		anims[animSelect]->parts[partSelect]->dataR.insert(index, pair<float, Vector3>(timeSelect, temp));
	//		anims[animSelect]->flagR.push_back(pair<int, Sprite*>(partSelect, new Sprite(fileR, shader, Vector2(timeLineSelect->Position().x - Width * 0.5f, 300 - 120 - Height * 0.5f), Vector2(0.25f, 0.25f))));
	//	}
	//}
}
void S00_AnimCustom::Move()
{
	Vector2 temp = Vector2(Mouse->Position().x - mouseLog.x, mouseLog.y - Mouse->Position().y) * scroll + parts[partSelect]->Position();

	if (phase == 0)
	{
		partDatas[partSelect]->T = temp;

		parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);
		direction->Position(parts[partSelect]->World()._41, parts[partSelect]->World()._42);
	}
	else if(animNum > 0)
	{
		bool New = true;
		vector<pair<float, Vector2>>::iterator index = anims[animSelect]->parts[partSelect]->dataT.begin();
		vector<pair<int, Sprite*>>::iterator indexSprite = anims[animSelect]->flagT.begin();
		//int index = 0;
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataT.size(); i++)
		{
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataT[i].first) { index++; indexSprite++; }

			if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataT[i].first
				&& anims[animSelect]->parts[partSelect]->dataT[i].first < timeSelect + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataT[i].second = temp;
				New = false;
				break;
			}
		}
		if (New) 
		{
			anims[animSelect]->parts[partSelect]->dataT.insert(index, pair<float, Vector2>(timeSelect, temp));
			anims[animSelect]->flagT.insert(indexSprite, pair<int, Sprite*>(partSelect, new Sprite(fileT, shader, Vector2(timeLineSelect->Position().x - FixWidth * 0.5f, 300 - 60 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));

			for (UINT i = 0; i < anims[animSelect]->flagT.size(); i++)
				anims[animSelect]->flagT[i].second->Update(fixedView, fixedProj);
		}
	}
}
void S00_AnimCustom::Stretch()
{
	Vector2 nonRot = Vector2(Mouse->Position().x - mouseLog.x, mouseLog.y - Mouse->Position().y);
	nonRot *= 0.01f;
	Vector2 temp;
	float angle = -dataSelect->D; //parts[partSelect]->Rotation().z + 
	//float mouseAngle = parts[partSelect]->Rotation().z + dataSelect->D;
	temp.x = cosf(angle) * nonRot.x + sinf(angle) * nonRot.y;
	temp.y = cosf(angle) * nonRot.y - sinf(angle) * nonRot.x;
	temp.x += parts[partSelect]->Scale().x;
	temp.y += parts[partSelect]->Scale().y;

	if (phase == 0)
	{
		partDatas[partSelect]->S = temp;
	
		parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);
	}
	else if (animNum > 0)
	{
		bool New = true;
		vector<pair<float, Vector2>>::iterator index = anims[animSelect]->parts[partSelect]->dataS.begin();
		vector<pair<int, Sprite*>>::iterator indexSprite = anims[animSelect]->flagS.begin();
		//int index = 0;
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataS.size(); i++)
		{
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataS[i].first) { index++; indexSprite++; }

			if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataS[i].first
				&& anims[animSelect]->parts[partSelect]->dataS[i].first < timeSelect + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataS[i].second = temp;
				New = false;
				break;
			}
		}
		if (New)
		{
			anims[animSelect]->parts[partSelect]->dataS.insert(index, pair<float, Vector2>(timeSelect, temp));
			anims[animSelect]->flagS.insert(indexSprite, pair<int, Sprite*>(partSelect, new Sprite(fileS, shader, Vector2(timeLineSelect->Position().x - FixWidth * 0.5f, 300 - 90 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));

			for (UINT i = 0; i < anims[animSelect]->flagS.size(); i++)
				anims[animSelect]->flagS[i].second->Update(fixedView, fixedProj);
		}
	}
}
void S00_AnimCustom::Rotate()
{
	Vector3 temp = Vector3(0, 0, Mouse->Position().x - mouseLog.x);
	temp *= 0.01f;
	temp.z += parts[partSelect]->Rotation().z;

	if (phase == 0)
	{
		partDatas[partSelect]->R = temp;
	
		parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);
	}
	else if (animNum > 0)
	{
		bool New = true;
		vector<pair<float, Vector3>>::iterator index = anims[animSelect]->parts[partSelect]->dataR.begin();
		vector<pair<int, Sprite*>>::iterator indexSprite = anims[animSelect]->flagR.begin();
		//int index = 0;
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataR.size(); i++)
		{
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataR[i].first) { index++; indexSprite++; }

			if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataR[i].first
				&& anims[animSelect]->parts[partSelect]->dataR[i].first < timeSelect + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataR[i].second = temp;
				New = false;
				break;
			}
		}
		if (New)
		{
			anims[animSelect]->parts[partSelect]->dataR.insert(index, pair<float, Vector3>(timeSelect, temp));
			anims[animSelect]->flagR.insert(indexSprite, pair<int, Sprite*>(partSelect, new Sprite(fileR, shader, Vector2(timeLineSelect->Position().x - FixWidth * 0.5f, 300 - 120 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));

			for (UINT i = 0; i < anims[animSelect]->flagR.size(); i++)
				anims[animSelect]->flagR[i].second->Update(fixedView, fixedProj);
		}

	}
}

void S00_AnimCustom::TimeShift()
{
	float Axis = Math::Clamp(Mouse->Position().x, 100.0f, 1920.0f);
	timeSelect = (Axis - 100) / 1800 * 4.5f;
	timeSelect = floorf(timeSelect * 20) * 0.05f;
	timeLineSelect->Position(Vector2(timeSelect / 4.5f * 1800 + 110, 0));

	timeLineSelect->Update(fixedView, fixedProj);
	UpdateSprite();
}
void S00_AnimCustom::TimeMaxShift()
{
	float Axis = Math::Clamp(Mouse->Position().x, 100.0f, 1920.0f);
	anims[animSelect]->timeMax = (Axis - 100) / 1800 * 4.5f;
	anims[animSelect]->timeMax = floorf(anims[animSelect]->timeMax * 20) * 0.05f;

	timeLineMax->Update(fixedView, fixedProj);
}
void S00_AnimCustom::DeleteFlag()
{
	float AxisX = Math::Clamp(Mouse->Position().x, 120.0f, 1920.0f);
	AxisX -= 100;
	AxisX = floorf(AxisX  * 0.05f) * 0.05f;

	if (Mouse->Position().y < (float)(Height - 300 + 45) || (float)(Height - 300 + 135) < Mouse->Position().y)
	{
		state = STATE::disable;
		return;
	}

	float AxisY = Mouse->Position().y;
	AxisY -= (float)(Height - 300 + 45);
	AxisY = floorf(AxisY * 0.03333f);

	
	if (0 - 1e-6f < AxisY && AxisY < 0 + 1e-6f)
	{
		int countT = 0;
		int count = 0;

		vector<pair<float, Vector2>>::iterator indexT = anims[animSelect]->parts[partSelect]->dataT.begin();
		for (indexT; indexT != anims[animSelect]->parts[partSelect]->dataT.end(); indexT++)
		{
			if (AxisX - 1e-3f < (*indexT).first && (*indexT).first < AxisX + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataT.erase(indexT);
				break;
			}
			countT++;
		}
		vector<pair<int, Sprite*>>::iterator index = anims[animSelect]->flagT.begin();
		for (index; index != anims[animSelect]->flagT.end(); index++)
		{
			if ((*index).first == partSelect)
			{
				if (count == countT)
				{
					SafeDelete((*index).second);
					anims[animSelect]->flagT.erase(index);
					break;
				}
				count++;
			}
		}
	}
	else if (1 - 1e-6f < AxisY && AxisY < 1 + 1e-6f)
	{
		int countS = 0;
		int count = 0;

		vector<pair<float, Vector2>>::iterator indexS = anims[animSelect]->parts[partSelect]->dataS.begin();
		for (indexS; indexS != anims[animSelect]->parts[partSelect]->dataS.end(); indexS++)
		{
			if (AxisX - 1e-3f < (*indexS).first && (*indexS).first < AxisX + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataS.erase(indexS);
				break;
			}
			countS++;
		}
		vector<pair<int, Sprite*>>::iterator index = anims[animSelect]->flagS.begin();
		for (index; index != anims[animSelect]->flagS.end(); index++)
		{
			if ((*index).first == partSelect)
			{
				if (count == countS)
				{
					SafeDelete((*index).second);
					anims[animSelect]->flagS.erase(index);
					break;
				}
				count++;
			}
		}
	}
	else if (2 - 1e-6f < AxisY && AxisY < 2 + 1e-6f)
	{
		int countR = 0;
		int count = 0;

		vector<pair<float, Vector3>>::iterator indexR = anims[animSelect]->parts[partSelect]->dataR.begin();
		for (indexR; indexR != anims[animSelect]->parts[partSelect]->dataR.end(); indexR++)
		{
			if (AxisX - 1e-3f < (*indexR).first && (*indexR).first < AxisX + 1e-3f)
			{
				anims[animSelect]->parts[partSelect]->dataR.erase(indexR);
				break;
			}
			countR++;
		}
		vector<pair<int, Sprite*>>::iterator index = anims[animSelect]->flagR.begin();
		for (index; index != anims[animSelect]->flagR.end(); index++)
		{
			if ((*index).first == partSelect)
			{
				if (count == countR)
				{
					SafeDelete((*index).second);
					anims[animSelect]->flagR.erase(index);
					break;
				}
				count++;
			}
		}
	}

	for (int i = 0; i < animNum; i++)
	{
		for (UINT j = 0; j < anims[i]->flagS.size(); j++)
			anims[i]->flagS[j].second->Update(fixedView, fixedProj);
		for (UINT j = 0; j < anims[i]->flagR.size(); j++)
			anims[i]->flagR[j].second->Update(fixedView, fixedProj);
		for (UINT j = 0; j < anims[i]->flagT.size(); j++)
			anims[i]->flagT[j].second->Update(fixedView, fixedProj);
	}

	state = STATE::disable;
	return;
}
void S00_AnimCustom::Reset()
{
	float AxisX = Math::Clamp(Mouse->Position().x, 100.0f, 1920.0f);
	AxisX -= 100;
	AxisX = floorf(AxisX  * 0.05f) * 0.05f;

	if (Mouse->Position().y < (float)(Height - 300 + 45) || (float)(Height - 300 + 135) < Mouse->Position().y)
	{
		state = STATE::disable;
		return;
	}

	float AxisY = Mouse->Position().y;
	AxisY -= (float)(Height - 300 + 45);
	AxisY = floorf(AxisY * 0.03333f);


	if (0 - 1e-6f < AxisY && AxisY < 0 + 1e-6f)
	{
		vector<pair<float, Vector2>>::iterator indexT = anims[animSelect]->parts[partSelect]->dataT.begin();
		for (indexT; indexT != anims[animSelect]->parts[partSelect]->dataT.end(); indexT++)
		{
			if (AxisX - 1e-3f < (*indexT).first && (*indexT).first < AxisX + 1e-3f)
			{
				(*indexT).second = partDatas[partSelect]->T;
				break;
			}
		}
	}
	else if (1 - 1e-6f < AxisY && AxisY < 1 + 1e-6f)
	{
		vector<pair<float, Vector2>>::iterator indexS = anims[animSelect]->parts[partSelect]->dataS.begin();
		for (indexS; indexS != anims[animSelect]->parts[partSelect]->dataS.end(); indexS++)
		{
			if (AxisX - 1e-3f < (*indexS).first && (*indexS).first < AxisX + 1e-3f)
			{
				(*indexS).second = partDatas[partSelect]->S;
				break;
			}
		}
	}
	else if (2 - 1e-6f < AxisY && AxisY < 2 + 1e-6f)
	{
		vector<pair<float, Vector3>>::iterator indexR = anims[animSelect]->parts[partSelect]->dataR.begin();
		for (indexR; indexR != anims[animSelect]->parts[partSelect]->dataR.end(); indexR++)
		{
			if (AxisX - 1e-3f < (*indexR).first && (*indexR).first < AxisX + 1e-3f)
			{
				(*indexR).second = partDatas[partSelect]->R;
				break;
			}
		}
	}

	state = STATE::disable;
	return;
}



void S00_AnimCustom::Render()
{
	// Render
	for (Sprite* part : parts)
		part->Render();

	// 임시
	//for (int i = 0; i < TempLines.size(); i++)
	//	TempLines[i]->Render();

	// Center
	for (int i = 0; i < 2; i++)
		centerLine[i]->Render();
	
	direction->Render();

	// ImGui
	//ImGui::SetWindowFontScale(1.2f);
	ImGui::Combo("phase", &phase, phaseMenu, ARRAYSIZE(phaseMenu));

	if (phase == 0) PhaseData();
	else if (phase == 1) PhaseAnim();
}

void S00_AnimCustom::PhaseData()
{
	if (ImGui::Button("Save Data"))
		SaveData(fileName);
	ImGui::SameLine();
	if (ImGui::Button("Load Data"))
		LoadData(fileName);
	
	ImGui::Separator();

	int temp = partSelect;
	ImGui::SliderInt("change Part", &partSelect, 0, partNum - 1);
	ImGui::Text("Part Name : %s", partDatas[partSelect]->name.c_str());

	if (temp != partSelect)
	{
		direction->Position(parts[partSelect]->World()._41, parts[partSelect]->World()._42);
		direction->Rotation(-partDatas[partSelect]->D);
		UpdateWorld(partDatas[0], parts[0]);
	}

	dataSelect = partDatas[partSelect];
	ImGui::InputFloat("Direction", &dataSelect->D);
	ImGui::InputFloat2("Position", dataSelect->T);
	ImGui::InputFloat2("Scale", dataSelect->S);
	ImGui::InputFloat3("Rotate", dataSelect->R);

	int parent = dataSelect->parent;
	ImGui::Checkbox("IsHide", &dataSelect->isHide);
	ImGui::SliderInt("Parent", &parent, 0, partNum - 1);

	parts[partSelect]->IsHide(dataSelect->isHide);
	if (partSelect != 0 && parent != dataSelect->parent && CheckChild(partSelect, parent)) 
		ChangeParent(parent);

	DWrite::GetDC()->BeginDraw();
	RECT rect = { 0, 0, 300, 0 };
	wstring str = L"";
	DWrite::RenderText(str + L"Alt   = 기준방향 변경", rect); rect.top += 15; rect.bottom += 15;
	DWrite::RenderText(str + L"Shift = 회전", rect); rect.top += 15; rect.bottom += 15;
	DWrite::RenderText(str + L"Ctrl  = 확대", rect); rect.top += 15; rect.bottom += 15;
	DWrite::RenderText(str + L"Drag  = 이동", rect);
	DWrite::GetDC()->EndDraw();

	//부모 자식관계 그래프
	ImGui::Begin("Tree");
	RenderTree(partDatas[0], 0);
	ImGui::End();
}
void S00_AnimCustom::PhaseAnim()
{
	if (ImGui::Button("Save Anim"))
		SaveAnim(fileName);
	ImGui::SameLine();
	if (ImGui::Button("Load Anim"))
	{
		LoadAnim(fileName);

		for (int i = 0; i < animNum; i++)
		{
			for (UINT j = 0; j < anims[i]->flagS.size(); j++)
				anims[i]->flagS[j].second->Update(fixedView, fixedProj);
			for (UINT j = 0; j < anims[i]->flagR.size(); j++)
				anims[i]->flagR[j].second->Update(fixedView, fixedProj);
			for (UINT j = 0; j < anims[i]->flagT.size(); j++)
				anims[i]->flagT[j].second->Update(fixedView, fixedProj);
		}
	}

	// 애니메이션 생성 버튼
	if (ImGui::Button("Create new Anim"))
		CreateAnim();
	
	if (animNum > 0)
	{
		int selTemp = animSelect;
		ImGui::SliderInt("change Anim", &animSelect, 0, animNum - 1);
		if (selTemp != animSelect) UpdateSprite();

		ImGui::Text("Anim Name : %s", anims[animSelect]->name.c_str());

		// 애니메이션 이름 변경
		if (ImGui::Button("Change Anim Name"))
		{
			anims[animSelect]->name = buf;
			ZeroMemory(buf, sizeof(buf));
		}
		ImGui::SameLine();
		ImGui::InputText("Input Name", buf, sizeof(buf));
		ImGui::Separator();

		if (ImGui::Button("Play")) { timePlay = 0.0f; isPlay = true; }
		ImGui::SameLine();
		if (ImGui::Button("Pause")) { Pause(); isPlay = false; }

		// part
		int temp = partSelect;
		ImGui::SliderInt("change Part", &partSelect, 0, partNum - 1);
		ImGui::Text("Part Name : %s", partDatas[partSelect]->name.c_str());

		if (temp != partSelect)
		{
			direction->Position(parts[partSelect]->World()._41, parts[partSelect]->World()._42);
			direction->Rotation(-partDatas[partSelect]->D);
			UpdateWorld(partDatas[0], parts[0]);
		}

		ImGui::Text("Time Max : %f", anims[animSelect]->timeMax);
		ImGui::Text("Time Select : %f", timeSelect);
		
		// SRT
		int s = 0, r = 0, t = 0, h = -1;
		Vector2 vs = {1, 1};
		Vector3 vr = {0, 0, 0};
		Vector2 vt = {0, 0};

		// T 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataT.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataT[i].first)
				t++;
		if (t == anims[animSelect]->parts[partSelect]->dataT.size())
		{
			vt = anims[animSelect]->parts[partSelect]->dataT[t - 1].second;
			ImGui::LabelText("Position", "%f\t%f", vt.x, vt.y);
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataT[t].first
			&& anims[animSelect]->parts[partSelect]->dataT[t].first < timeSelect + 1e-3f)
		{
			ImGui::InputFloat2("Position", anims[animSelect]->parts[partSelect]->dataT[t].second);
			vt = anims[animSelect]->parts[partSelect]->dataT[t].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[partSelect]->dataT[t].first - anims[animSelect]->parts[partSelect]->dataT[t - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[partSelect]->dataT[t - 1].first) / delta;
			
			vt = anims[animSelect]->parts[partSelect]->dataT[t - 1].second * (1 - delta)
				+ anims[animSelect]->parts[partSelect]->dataT[t].second * delta;
			ImGui::LabelText("Position", "%f\t%f", vt.x, vt.y);
		}
		
		// R 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataR.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataR[i].first)
				r++;
		if (r == anims[animSelect]->parts[partSelect]->dataR.size())
		{
			vr = anims[animSelect]->parts[partSelect]->dataR[r - 1].second;
			ImGui::LabelText("rotate", "%f\t%f\t%f", vr.x, vr.y, vr.z);
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataR[r].first
			&& anims[animSelect]->parts[partSelect]->dataR[r].first < timeSelect + 1e-3f)
		{
			ImGui::InputFloat3("rotate", anims[animSelect]->parts[partSelect]->dataR[r].second);
			vr = anims[animSelect]->parts[partSelect]->dataR[r].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[partSelect]->dataR[r].first - anims[animSelect]->parts[partSelect]->dataR[r - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[partSelect]->dataR[r - 1].first) / delta;

			vr = anims[animSelect]->parts[partSelect]->dataR[r - 1].second * (1 - delta)
				+ anims[animSelect]->parts[partSelect]->dataR[r].second * delta;
			ImGui::LabelText("rotate", "%f\t%f\t%f", vr.x, vr.y, vr.z);
		}

		// S 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataS.size(); i++)
			if (timeSelect - 1e-3f > anims[animSelect]->parts[partSelect]->dataS[i].first)
				s++;
		if (s == anims[animSelect]->parts[partSelect]->dataS.size())
		{
			vs = anims[animSelect]->parts[partSelect]->dataS[s - 1].second;
			ImGui::LabelText("Scale", "%f\t%f", vs.x, vs.y);
		}
		else if (timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataS[s].first
			&& anims[animSelect]->parts[partSelect]->dataS[s].first < timeSelect + 1e-3f)
		{
			ImGui::InputFloat2("Scale", anims[animSelect]->parts[partSelect]->dataS[s].second);
			vs = anims[animSelect]->parts[partSelect]->dataS[s].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[partSelect]->dataS[s].first - anims[animSelect]->parts[partSelect]->dataS[s - 1].first;
			delta = (timeSelect - anims[animSelect]->parts[partSelect]->dataS[s - 1].first) / delta;

			vs = anims[animSelect]->parts[partSelect]->dataS[s - 1].second * (1 - delta)
				+ anims[animSelect]->parts[partSelect]->dataS[s].second * delta;
			ImGui::LabelText("Scale", "%f\t%f", vs.x, vs.y);
		}

		for (UINT i = 0; i < anims[animSelect]->parts[partSelect]->dataH.size(); i++)
			if (timeSelect + 1e-3f > anims[animSelect]->parts[partSelect]->dataH[i].first)
				h++;

		if(timeSelect - 1e-3f < anims[animSelect]->parts[partSelect]->dataH[h].first
			&& anims[animSelect]->parts[partSelect]->dataH[h].first < timeSelect + 1e-3f)
				ImGui::Checkbox("IsHide", &anims[animSelect]->parts[partSelect]->dataH[h].second);
		else if (ImGui::Button("IsHide"))
		{
			anims[animSelect]->parts[partSelect]->dataH.push_back(pair<float, bool>{ timeSelect, false });
		}

		parts[partSelect]->IsHide(anims[animSelect]->parts[partSelect]->dataH[h].second);



		// 적용
		parts[partSelect]->World(vs, vr, vt);
		UpdateWorld(partDatas[partSelect], parts[partSelect]);

		// 타임테이블 배경 & 라인
		tableBackground->Render();
		for (Line* line : timeHLines)
			line->Render();
		for (Line* line : timeVLines)
			line->Render();
		timeLineMax->Render();
		timeLineSelect->Render();

		DWrite::GetDC()->BeginDraw(); // 텍스트 드로우 시작
		RECT rect = { 0, 0, 300, 0 };
		wstring str = L"";
		DWrite::RenderText(str + L"Alt   = 기준방향 변경", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Shift = 회전", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Ctrl  = 확대", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Drag  = 이동", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Shift  = 키스톤 초기화", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Ctrl  = 키스톤 삭제", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Tab  = 애니메이션 길이 변경", rect); rect.top += 15; rect.bottom += 15;
		DWrite::RenderText(str + L"Drag  = 편집중인 시간 변경", rect);
		
		rect = { 97, (int)(Height - 280), 140, (int)(Height - 280)};
		for (int i = 0; i < 10; i++)
		{
			str = to_wstring(i / 2);
			i % 2 == 0 ? DWrite::RenderText(str + L".0", rect) : DWrite::RenderText(str + L".5", rect);
			rect.left += 200; rect.right += 200;
		}
		rect.left = 10; rect.right = 100;
		rect.top += 30; rect.bottom += 30;
		DWrite::RenderText(str = L"Position", rect);
		rect.top += 30; rect.bottom += 30;
		DWrite::RenderText(str = L"Scale", rect);
		rect.top += 30; rect.bottom += 30;
		DWrite::RenderText(str = L"Rotate", rect);
		
		DWrite::GetDC()->EndDraw(); // 텍스트 드로우 종료

		// flag
		for (UINT i = 0; i < anims[animSelect]->flagS.size(); i++)
			if (anims[animSelect]->flagS[i].first == partSelect) anims[animSelect]->flagS[i].second->Render();
		for (UINT i = 0; i < anims[animSelect]->flagR.size(); i++)
			if(anims[animSelect]->flagR[i].first == partSelect) anims[animSelect]->flagR[i].second->Render();
		for (UINT i = 0; i < anims[animSelect]->flagT.size(); i++)
			if(anims[animSelect]->flagT[i].first == partSelect) anims[animSelect]->flagT[i].second->Render();
	}

	else
	{
		DWrite::GetDC()->BeginDraw();
		RECT rect = { 0, 0, 500, 0 };
		wstring str = L"";
		DWrite::RenderText(str + L"Create Anim 버튼을 눌러 새 애니메이션을 생성해주세요.", rect);
		DWrite::GetDC()->EndDraw();
	}
}
void S00_AnimCustom::RenderTree(DataSet * set, int depth)
{
	int count = set->child.size();
	string s = "";
	for (int i = 0; i < depth; i++) s += "  ";
	ImGui::Text("%s", (s + set->name).c_str());

	depth++;

	for (int i = 0; i < count; i++)
		RenderTree(partDatas[set->child[i]], depth);
}
void S00_AnimCustom::CreateAnim()
{
	int i = animNum;
	animNum++;
	anims.push_back(new AnimData());
	
	anims[i]->partNum = partNum;
	for (int k = 0; k < partNum; k++)
	{
		anims[i]->parts.push_back(new Part());
		anims[i]->parts[k]->dataS.push_back(pair<float, Vector2>(0.0f, partDatas[k]->S));
		anims[i]->parts[k]->dataR.push_back(pair<float, Vector3>(0.0f, partDatas[k]->R));
		anims[i]->parts[k]->dataT.push_back(pair<float, Vector2>(0.0f, partDatas[k]->T));
		anims[i]->parts[k]->dataH.push_back(pair<float, bool>(0.0f, partDatas[k]->isHide));

		anims[i]->flagS.push_back(pair<int, Sprite*>(k, new Sprite(fileS, shader, Vector2(110 - FixWidth * 0.5f, 300 - 90 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
		anims[i]->flagR.push_back(pair<int, Sprite*>(k, new Sprite(fileR, shader, Vector2(110 - FixWidth * 0.5f, 300 - 120 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
		anims[i]->flagT.push_back(pair<int, Sprite*>(k, new Sprite(fileT, shader, Vector2(110 - FixWidth * 0.5f, 300 - 60 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
	}

	anims[i]->flagS[0].second->Update(fixedView, fixedProj);
	anims[i]->flagR[0].second->Update(fixedView, fixedProj);
	anims[i]->flagT[0].second->Update(fixedView, fixedProj);

	animSelect = i;
	anims[i]->timeMax = 1.0f;
	timeLineMax->Position(Vector2(anims[i]->timeMax / 4.5f * 1800 + 110, 0));

	anims[i]->name = "newAnim";
}

void S00_AnimCustom::Play()
{
	if (!isPlay) return;

	timePlay += Time::Delta();
	if (timePlay > anims[animSelect]->timeMax) timePlay -= anims[animSelect]->timeMax;
	timeLineSelect->Position(Vector2(timePlay / 4.5f * 1800 + 110, 0));

	timeLineSelect->Update(fixedView, fixedProj);

	// SRT
	int s, r, t;
	Vector2 vs = { 1, 1 };
	Vector3 vr = { 0, 0, 0 };
	Vector2 vt = { 0, 0 };

	for (UINT p = 0; p < partDatas.size(); p++)
	{
		s = 0, r = 0, t = 0;

		// T 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataT.size(); i++)
			if (timePlay - 1e-3f > anims[animSelect]->parts[p]->dataT[i].first)
				t++;
		if (t == anims[animSelect]->parts[p]->dataT.size())
		{
			vt = anims[animSelect]->parts[p]->dataT[t - 1].second;
		}
		else if (timePlay - 1e-3f < anims[animSelect]->parts[p]->dataT[t].first
			&& anims[animSelect]->parts[p]->dataT[t].first < timePlay + 1e-3f)
		{
			vt = anims[animSelect]->parts[p]->dataT[t].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataT[t].first - anims[animSelect]->parts[p]->dataT[t - 1].first;
			delta = (timePlay - anims[animSelect]->parts[p]->dataT[t - 1].first) / delta;

			vt = anims[animSelect]->parts[p]->dataT[t - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataT[t].second * delta;
		}

		// R 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataR.size(); i++)
			if (timePlay - 1e-3f > anims[animSelect]->parts[p]->dataR[i].first)
				r++;
		if (r == anims[animSelect]->parts[p]->dataR.size())
		{
			vr = anims[animSelect]->parts[p]->dataR[r - 1].second;
		}
		else if (timePlay - 1e-3f < anims[animSelect]->parts[p]->dataR[r].first
			&& anims[animSelect]->parts[p]->dataR[r].first < timePlay + 1e-3f)
		{
			vr = anims[animSelect]->parts[p]->dataR[r].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataR[r].first - anims[animSelect]->parts[p]->dataR[r - 1].first;
			delta = (timePlay - anims[animSelect]->parts[p]->dataR[r - 1].first) / delta;

			vr = anims[animSelect]->parts[p]->dataR[r - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataR[r].second * delta;
		}

		// S 선형보간
		for (UINT i = 0; i < anims[animSelect]->parts[p]->dataS.size(); i++)
			if (timePlay - 1e-3f > anims[animSelect]->parts[p]->dataS[i].first)
				s++;
		if (s == anims[animSelect]->parts[p]->dataS.size())
		{
			vs = anims[animSelect]->parts[p]->dataS[s - 1].second;
		}
		else if (timePlay - 1e-3f < anims[animSelect]->parts[p]->dataS[s].first
			&& anims[animSelect]->parts[p]->dataS[s].first < timePlay + 1e-3f)
		{
			vs = anims[animSelect]->parts[p]->dataS[s].second;
		}
		else
		{
			float delta = anims[animSelect]->parts[p]->dataS[s].first - anims[animSelect]->parts[p]->dataS[s - 1].first;
			delta = (timePlay - anims[animSelect]->parts[p]->dataS[s - 1].first) / delta;

			vs = anims[animSelect]->parts[p]->dataS[s - 1].second * (1 - delta)
				+ anims[animSelect]->parts[p]->dataS[s].second * delta;
		}
		// 적용
		parts[p]->World(vs, vr, vt);
	}
	UpdateWorld(partDatas[0], parts[0]);
}
void S00_AnimCustom::Pause()
{
	timeLineSelect->Position(Vector2(timeSelect / 4.5f * 1800 + 110, 0));

	timeLineSelect->Update(fixedView, fixedProj);
	UpdateSprite();
	//// SRT
	//int s, r, t;
	//Vector2 vs = { 1, 1 };
	//Vector3 vr = { 0, 0, 0 };
	//Vector2 vt = { 0, 0 };
	//
	//for (UINT p = 0; p < partDatas.size(); p++)
	//{
	//	s = 0, r = 0, t = 0;
	//
	//	// T 선형보간
	//	for (UINT i = 0; i < anims[animSelect]->parts[p]->dataT.size(); i++)
	//		if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataT[i].first)
	//			t++;
	//	if (t == anims[animSelect]->parts[p]->dataT.size())
	//	{
	//		vt = anims[animSelect]->parts[p]->dataT[t - 1].second;
	//	}
	//	else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataT[t].first
	//		&& anims[animSelect]->parts[p]->dataT[t].first < timeSelect + 1e-3f)
	//	{
	//		vt = anims[animSelect]->parts[p]->dataT[t].second;
	//	}
	//	else
	//	{
	//		float delta = anims[animSelect]->parts[p]->dataT[t].first - anims[animSelect]->parts[p]->dataT[t - 1].first;
	//		delta = (timeSelect - anims[animSelect]->parts[p]->dataT[t - 1].first) / delta;
	//
	//		vt = anims[animSelect]->parts[p]->dataT[t - 1].second * (1 - delta)
	//			+ anims[animSelect]->parts[p]->dataT[t].second * delta;
	//	}
	//
	//	// R 선형보간
	//	for (UINT i = 0; i < anims[animSelect]->parts[p]->dataR.size(); i++)
	//		if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataR[i].first)
	//			r++;
	//	if (r == anims[animSelect]->parts[p]->dataR.size())
	//	{
	//		vr = anims[animSelect]->parts[p]->dataR[r - 1].second;
	//	}
	//	else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataR[r].first
	//		&& anims[animSelect]->parts[p]->dataR[r].first < timeSelect + 1e-3f)
	//	{
	//		vr = anims[animSelect]->parts[p]->dataR[r].second;
	//	}
	//	else
	//	{
	//		float delta = anims[animSelect]->parts[p]->dataR[r].first - anims[animSelect]->parts[p]->dataR[r - 1].first;
	//		delta = (timeSelect - anims[animSelect]->parts[p]->dataR[r - 1].first) / delta;
	//
	//		vr = anims[animSelect]->parts[p]->dataR[r - 1].second * (1 - delta)
	//			+ anims[animSelect]->parts[p]->dataR[r].second * delta;
	//	}
	//
	//	// S 선형보간
	//	for (UINT i = 0; i < anims[animSelect]->parts[p]->dataS.size(); i++)
	//		if (timeSelect - 1e-3f > anims[animSelect]->parts[p]->dataS[i].first)
	//			s++;
	//	if (s == anims[animSelect]->parts[p]->dataS.size())
	//	{
	//		vs = anims[animSelect]->parts[p]->dataS[s - 1].second;
	//	}
	//	else if (timeSelect - 1e-3f < anims[animSelect]->parts[p]->dataS[s].first
	//		&& anims[animSelect]->parts[p]->dataS[s].first < timeSelect + 1e-3f)
	//	{
	//		vs = anims[animSelect]->parts[p]->dataS[s].second;
	//	}
	//	else
	//	{
	//		float delta = anims[animSelect]->parts[p]->dataS[s].first - anims[animSelect]->parts[p]->dataS[s - 1].first;
	//		delta = (timeSelect - anims[animSelect]->parts[p]->dataS[s - 1].first) / delta;
	//
	//		vs = anims[animSelect]->parts[p]->dataS[s - 1].second * (1 - delta)
	//			+ anims[animSelect]->parts[p]->dataS[s].second * delta;
	//	}
	//	// 적용
	//	parts[p]->World(vs, vr, vt);
	//}
	//UpdateWorld(partDatas[0], parts[0]);
}

bool S00_AnimCustom::CheckChild(int index, int target)
{
	if (index == target) return false;

	bool result = true;
	int count = partDatas[index]->child.size();
	if (count < 1) return true;

	for (int i = 0; i < count; i++)
		result &= CheckChild(partDatas[index]->child[i], target);

	return result;
}
void S00_AnimCustom::ChangeParent(int target)
{
	// 원래 부모한테서 자신 제거
	vector<int>::iterator iter = partDatas[dataSelect->parent]->child.begin();
	vector<int>::iterator end = partDatas[dataSelect->parent]->child.end();
	
	for (iter; iter != end; iter++)
	{
		if(*iter == partSelect)
		{
			partDatas[dataSelect->parent]->child.erase(iter);
			break;
		}
	}

	// 이번엔 새 부모한테 자신 추가
	dataSelect->parent = target;
	partDatas[target]->child.push_back(partSelect);

	//parts[partSelect]->World(dataSelect->S, dataSelect->R, dataSelect->T);
	UpdateWorld(partDatas[target], parts[target]);
}



void S00_AnimCustom::SaveData(string fileName)
{
	fopen_s(&file, ("../../Image/Character/" + fileName + "/Data.txt").c_str(), "w");

	fprintf_s(file, "C : %d\n", partNum);
	for (int i = 0; i < partNum; i++)
	{
		fprintf_s(file, "N : %s\n", partDatas[i]->name.c_str());
		fprintf_s(file, "D : %f\n", partDatas[i]->D);
		fprintf_s(file, "S : %f, %f\n", partDatas[i]->S.x, partDatas[i]->S.y);
		fprintf_s(file, "R : %f, %f, %f\n", partDatas[i]->R.x, partDatas[i]->R.y, partDatas[i]->R.z);
		fprintf_s(file, "T : %f, %f\n", partDatas[i]->T.x, partDatas[i]->T.y);

		fprintf_s(file, "H : %d\n", partDatas[i]->isHide);
		fprintf_s(file, "P : %d\n", partDatas[i]->parent);
	}

	fclose(file);
}
void S00_AnimCustom::LoadData(string fileName)
{
	fopen_s(&file, ("../../Image/Character/" + fileName + "/Data.txt").c_str(), "r");
	
	fscanf_s(file, "C : %d\n", &partNum);
	
	// 이미지가 1개도 등록되어있지 않다면 에러
	assert(partNum > 0);

	// 로드를 위한 초기화
	if (parts.size() > 0)
	{
		for (int i = 0; i < partNum; i++)
		{
			SafeDelete(parts[i]);
			SafeDelete(partDatas[i]);
		}
		parts.clear();
		partDatas.clear();
	}
	
	// 이미지 데이터 불러오기
	char name[32];
	for (int i = 0; i < partNum; i++)
	{
		partDatas.push_back(new DataSet());

		fscanf_s(file, "N : %s\n", name, sizeof(name));	partDatas[i]->name = name;
		fscanf_s(file, "D : %f\n", &partDatas[i]->D);
		fscanf_s(file, "S : %f, %f\n", &partDatas[i]->S.x, &partDatas[i]->S.y);
		fscanf_s(file, "R : %f, %f, %f\n", &partDatas[i]->R.x, &partDatas[i]->R.y, &partDatas[i]->R.z);
		fscanf_s(file, "T : %f, %f\n", &partDatas[i]->T.x, &partDatas[i]->T.y);

		fscanf_s(file, "H : %d\n", (int*)&partDatas[i]->isHide);
		fscanf_s(file, "P : %d\n", &partDatas[i]->parent);
		
		parts.push_back(new Sprite(String::ToWString("Character/" + fileName + "/" + partDatas[i]->name + ".png"), shader));
		parts[i]->World(partDatas[i]->S, partDatas[i]->R, partDatas[i]->T);
		parts[i]->Direction(partDatas[i]->D);
		parts[i]->IsHide(partDatas[i]->isHide);
	}

	// 차일드 데이터 갱신
	for (int i = 0; i < partNum; i++)
	{
		if(partDatas[i]->parent != -1)
			partDatas[partDatas[i]->parent]->child.push_back(i);
	}

	dataSelect = partDatas[0];

	fclose(file);
}
void S00_AnimCustom::SaveAnim(string fileName)
{
	fopen_s(&file, ("../../Image/Character/" + fileName + "/Anim.txt").c_str(), "w");

	fprintf_s(file, "C : %d\n", animNum);
	
	// 애니메이션 갯수만큼 저장
	for (int i = 0; i < animNum; i++)
	{
		fprintf_s(file, "N : %s\n", anims[i]->name.c_str());
		fprintf_s(file, "R : %d\n", anims[i]->isRoot);
		fprintf_s(file, "T : %f\n", anims[i]->timeMax);
		fprintf_s(file, "PC : %d\n", anims[i]->partNum);

		// 각 파츠의 갯수만큼 저장
		for (int j = 0; j < anims[i]->partNum; j++)
		{
			// S
			fprintf_s(file, "SC : %d\n", anims[i]->parts[j]->dataS.size());
			for (UINT k = 0; k < anims[i]->parts[j]->dataS.size(); k++)
			{
				fprintf_s(file, "ST : %f\n", anims[i]->parts[j]->dataS[k].first);
				fprintf_s(file, "SK : %f, %f\n", anims[i]->parts[j]->dataS[k].second.x, anims[i]->parts[j]->dataS[k].second.y);
			}
			// R
			fprintf_s(file, "RC : %d\n", anims[i]->parts[j]->dataR.size());
			for (UINT k = 0; k < anims[i]->parts[j]->dataR.size(); k++)
			{
				fprintf_s(file, "RT : %f\n", anims[i]->parts[j]->dataR[k].first);
				fprintf_s(file, "RK : %f, %f, %f\n", anims[i]->parts[j]->dataR[k].second.x, anims[i]->parts[j]->dataR[k].second.y, anims[i]->parts[j]->dataR[k].second.z);
			}
			// T
			fprintf_s(file, "TC : %d\n", anims[i]->parts[j]->dataT.size());
			for (UINT k = 0; k < anims[i]->parts[j]->dataT.size(); k++)
			{
				fprintf_s(file, "TT : %f\n", anims[i]->parts[j]->dataT[k].first);
				fprintf_s(file, "TK : %f, %f\n", anims[i]->parts[j]->dataT[k].second.x, anims[i]->parts[j]->dataT[k].second.y);
			}
			//H
			fprintf_s(file, "HC : %d\n", anims[i]->parts[j]->dataH.size());
			for (UINT k = 0; k < anims[i]->parts[j]->dataH.size(); k++)
			{
				fprintf_s(file, "HT : %f\n", anims[i]->parts[j]->dataH[k].first);
				anims[i]->parts[j]->dataH[k].second ? fprintf_s(file, "HK : %d\n", 1) : fprintf_s(file, "HK : %d\n", 0);
			}
		}
	}

	fclose(file);
}
void S00_AnimCustom::LoadAnim(string fileName)
{
	fopen_s(&file, ("../../Image/Character/" + fileName + "/Anim.txt").c_str(), "r");

	// 로드를 위한 초기화
	for (int i = 0; i < animNum; i++)
		SafeDelete(anims[i]);
	anims.clear();

	fscanf_s(file, "C : %d\n", &animNum);

	if (animNum <= animSelect)
		animSelect = animNum - 1;

	char name[32];
	int count;

	for (int i = 0; i < animNum; i++)
	{
		anims.push_back(new AnimData());

		fscanf_s(file, "N : %s\n", name, sizeof(name)); anims[i]->name = name;
		fscanf_s(file, "R : %d\n", (int*)&(anims[i]->isRoot));
		fscanf_s(file, "T : %f\n", &(anims[i]->timeMax));
		fscanf_s(file, "PC : %d\n", &(anims[i]->partNum));

		for (int j = 0; j < anims[i]->partNum; j++)
		{
			anims[i]->parts.push_back(new Part());

			// S
			fscanf_s(file, "SC : %d\n", &count);
			for (int k = 0; k < count; k++)
			{
				anims[i]->parts[j]->dataS.push_back(pair<float, Vector2>());
				fscanf_s(file, "ST : %f\n", &(anims[i]->parts[j]->dataS[k].first));
				fscanf_s(file, "SK : %f, %f\n", &(anims[i]->parts[j]->dataS[k].second.x), &(anims[i]->parts[j]->dataS[k].second.y));

				anims[i]->flagS.push_back(pair<int, Sprite*>(j, new Sprite(fileS, shader, Vector2(110 + anims[i]->parts[j]->dataS[k].first * 400 - FixWidth * 0.5f, 300 - 90 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
			}
			// R
			fscanf_s(file, "RC : %d\n", &count);
			for (int k = 0; k < count; k++)
			{
				anims[i]->parts[j]->dataR.push_back(pair<float, Vector3>());
				fscanf_s(file, "RT : %f\n", &(anims[i]->parts[j]->dataR[k].first));
				fscanf_s(file, "RK : %f, %f, %f\n", &(anims[i]->parts[j]->dataR[k].second.x), &(anims[i]->parts[j]->dataR[k].second.y), &(anims[i]->parts[j]->dataR[k].second.z));
				
				anims[i]->flagR.push_back(pair<int, Sprite*>(j, new Sprite(fileR, shader, Vector2(110 + anims[i]->parts[j]->dataR[k].first * 400 - FixWidth * 0.5f, 300 - 120 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
			}
			// T
			fscanf_s(file, "TC : %d\n", &count);
			for (int k = 0; k < count; k++)
			{
				anims[i]->parts[j]->dataT.push_back(pair<float, Vector2>());
				fscanf_s(file, "TT : %f\n", &(anims[i]->parts[j]->dataT[k].first));
				fscanf_s(file, "TK : %f, %f\n", &(anims[i]->parts[j]->dataT[k].second.x), &(anims[i]->parts[j]->dataT[k].second.y));

				anims[i]->flagT.push_back(pair<int, Sprite*>(j, new Sprite(fileT, shader, Vector2(110 + anims[i]->parts[j]->dataT[k].first * 400 - FixWidth * 0.5f, 300 - 60 - FixHeight * 0.5f), Vector2(0.25f, 0.25f))));
			}
			// H
			fscanf_s(file, "HC : %d\n", &count);
			int boolConv = 0;
			for (int k = 0; k < count; k++)
			{
				anims[i]->parts[j]->dataH.push_back(pair<float, Vector2>());
				fscanf_s(file, "HT : %f\n", &(anims[i]->parts[j]->dataH[k].first));
				fscanf_s(file, "HK : %d\n", &boolConv);
				anims[i]->parts[j]->dataH[k].second = boolConv == 0 ? false : true;
				//anims[i]->parts[j]->dataH[k].second = false;
			}
		}
	}

	fclose(file);
}

