#pragma once
#include "Scene.h"

enum STATE { none, disable, setDirection, move, stretch, rotate, timeShift, timeMaxShift, deleteFlag, reset};

class S00_AnimCustom : public Scene
{

public:
	struct DataSet;

	S00_AnimCustom(SceneValues* values);
	~S00_AnimCustom();

	void Update() override;

	void InputState();

	void UpdateWorld(DataSet * set, Sprite* sprite);
	void UpdateSprite();

	void SetDirection();
	void Move();
	void Stretch();
	void Rotate();

	void TimeShift();
	void TimeMaxShift();
	void DeleteFlag();
	void Reset();

	void Render() override;

	void PhaseData();
	void PhaseAnim();
	void RenderTree(DataSet * set, int depth);
	void CreateAnim();

	void Play();
	void Pause();

	bool CheckChild(int index, int target);
	void ChangeParent(int target);

	void SaveData(string fileName);
	void LoadData(string fileName);

	void SaveAnim(string fileName);
	void LoadAnim(string fileName);

	// 각 이미지 데이터셋
	struct DataSet
	{
		string name;		// 이름
		float D;			// 방향 - 이 방향에 따라 x축, y축 방향이 바뀐다.
		Vector2 S;			// 스케일
		Vector3 R;			// 로테이트
		Vector2 T;			// 포지션

		bool isHide;		// 보이는지 여부
		int parent;			// 부모 이미지
		vector<int> child;	// 자식 이미지

		//DataSet operator * (float& data)	// 곱셈
		//{
		//	DataSet result;
		//	result.S = S * data;
		//	result.R = R * data;
		//	result.T = T * data;
		//	return result;
		//}
		//DataSet operator + (DataSet& data)	// 덧셈
		//{
		//	DataSet result;
		//	result.S = S + data.S;
		//	result.R = R + data.R;
		//	result.T = T + data.T;
		//	return result;
		//}
	};

private:
	// 시간별 데이터를 담고 있는 이미지파트 1개
	struct Part
	{
		Part() : dataNow(1) {}

		// 제어변수
		int dataNow;		// 현재 인덱스값 (초기값 1)

		// 저장변수
		vector<pair<float, Vector2>> dataS;	// 시간에 따른 데이터 <시간, 스케일>
		vector<pair<float, Vector3>> dataR;	// 시간에 따른 데이터 <시간, 로테이트>
		vector<pair<float, Vector2>> dataT;	// 시간에 따른 데이터 <시간, 포지션>
		vector<pair<float, bool>> dataH;	// 시간에 따른 데이터 <시간, 포지션>
	};

	// 이미지파트들을 담고 있는 애니메이션 1개
	struct AnimData
	{
		AnimData() : isActive(false), timeNow(0), isRoot(true), timeMax(1) {}
		~AnimData()
		{
			for (int i = 0; i < partNum; i++)
				SafeDelete(parts[i]);
			for (UINT i = 0; i < flagS.size(); i++)
				SafeDelete(flagS[i].second);
			for (UINT i = 0; i < flagR.size(); i++)
				SafeDelete(flagR[i].second);
			for (UINT i = 0; i < flagT.size(); i++)
				SafeDelete(flagT[i].second);

			parts.clear();
			flagS.clear();
			flagR.clear();
			flagT.clear();
		}

		// 제어변수
		bool isActive;			// 애니메이션 활성화 여부 (초기값 false)
		float timeNow;			// 애니메이션 재생시작부터 흐른 시간(초) (초기값 0)

		// 저장변수
		string name;			// 애니메이션 이름
		bool isRoot;			// 애니메이션 반복재생 여부
		float timeMax;			// 애니메이션 재생 길이(초)

		int partNum;			// 이미지 갯수
		vector<Part*> parts;	// 이미지별 애니 데이터
		vector<pair<int, Sprite*>> flagS;	// 타임 플래그 <파츠 번호, 스케일>
		vector<pair<int, Sprite*>> flagR;	// 타임 플래그 <파츠 번호, 로테이트>
		vector<pair<int, Sprite*>> flagT;	// 타임 플래그 <파츠 번호, 포지션>
	};



private:
	FILE* file;					// 세이브/로드용 파일 포인터
	string fileName;			// 파일 이름
	wstring shader;				// 쉐이더 이름
	wstring fileS;				// 파일 이름 (스케일)
	wstring fileR;				// 파일 이름 (로테이트)
	wstring fileT;				// 파일 이름 (포지션)

	// 조작계 변수
	Matrix fixedView;			// 고정 카메라
	Matrix fixedProj;			// 고정 카메라
	float scroll;				// 확대 / 축소
	Vector2 mouseLog;			// 마우스 이전 위치
	STATE state;				// 현재 편집 상황
	int phase;					// 현재 편집중인 상황
	char* phaseMenu[2];			// 현재 상황 리스트
	int partSelect;				// 현재 선택된 이미지
	DataSet* dataSelect;		// 현재 선택된 이미지의 데이터
	int animSelect;				// 현재 선택된 애니메이션
	float timeSelect;			// 현재 선택된 시간
	float timePlay;				// 재생시간
	bool isPlay;				// 재생중인지

	// 로드 보조변수
	char buf[20];				// 스트링 데이터 로드를 위한 버퍼
	
	// Data.txt
	int partNum;				// 이미지 갯수
	vector<Sprite*> parts;		// 실제 이미지
	vector<DataSet*> partDatas;	// 이미지 초기 데이터

	// Anim.txt
	int animNum;				// 애니메이션 갯수
	vector<AnimData*> anims;	// 애니메이션 데이터

	// UI
	vector<class Line*> centerLine;		// 중앙선
	class Line* direction;				// 각 이미지 방향
	Sprite* tableBackground;			// 타임테이블 배경
	vector<class Line*> timeHLines;		// 타임테이블 라인 (가로선)
	vector<class Line*> timeVLines;		// 타임테이블 라인 (눈금)
	class Line* timeLineMax;			// 타임테이블 라인 (최종 시간)
	class Line* timeLineSelect;			// 타임테이블 라인 (현재 시간)

	//vector<class Line*> TempLines;
};



