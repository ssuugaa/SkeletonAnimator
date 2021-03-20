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

	// �� �̹��� �����ͼ�
	struct DataSet
	{
		string name;		// �̸�
		float D;			// ���� - �� ���⿡ ���� x��, y�� ������ �ٲ��.
		Vector2 S;			// ������
		Vector3 R;			// ������Ʈ
		Vector2 T;			// ������

		bool isHide;		// ���̴��� ����
		int parent;			// �θ� �̹���
		vector<int> child;	// �ڽ� �̹���

		//DataSet operator * (float& data)	// ����
		//{
		//	DataSet result;
		//	result.S = S * data;
		//	result.R = R * data;
		//	result.T = T * data;
		//	return result;
		//}
		//DataSet operator + (DataSet& data)	// ����
		//{
		//	DataSet result;
		//	result.S = S + data.S;
		//	result.R = R + data.R;
		//	result.T = T + data.T;
		//	return result;
		//}
	};

private:
	// �ð��� �����͸� ��� �ִ� �̹�����Ʈ 1��
	struct Part
	{
		Part() : dataNow(1) {}

		// �����
		int dataNow;		// ���� �ε����� (�ʱⰪ 1)

		// ���庯��
		vector<pair<float, Vector2>> dataS;	// �ð��� ���� ������ <�ð�, ������>
		vector<pair<float, Vector3>> dataR;	// �ð��� ���� ������ <�ð�, ������Ʈ>
		vector<pair<float, Vector2>> dataT;	// �ð��� ���� ������ <�ð�, ������>
		vector<pair<float, bool>> dataH;	// �ð��� ���� ������ <�ð�, ������>
	};

	// �̹�����Ʈ���� ��� �ִ� �ִϸ��̼� 1��
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

		// �����
		bool isActive;			// �ִϸ��̼� Ȱ��ȭ ���� (�ʱⰪ false)
		float timeNow;			// �ִϸ��̼� ������ۺ��� �帥 �ð�(��) (�ʱⰪ 0)

		// ���庯��
		string name;			// �ִϸ��̼� �̸�
		bool isRoot;			// �ִϸ��̼� �ݺ���� ����
		float timeMax;			// �ִϸ��̼� ��� ����(��)

		int partNum;			// �̹��� ����
		vector<Part*> parts;	// �̹����� �ִ� ������
		vector<pair<int, Sprite*>> flagS;	// Ÿ�� �÷��� <���� ��ȣ, ������>
		vector<pair<int, Sprite*>> flagR;	// Ÿ�� �÷��� <���� ��ȣ, ������Ʈ>
		vector<pair<int, Sprite*>> flagT;	// Ÿ�� �÷��� <���� ��ȣ, ������>
	};



private:
	FILE* file;					// ���̺�/�ε�� ���� ������
	string fileName;			// ���� �̸�
	wstring shader;				// ���̴� �̸�
	wstring fileS;				// ���� �̸� (������)
	wstring fileR;				// ���� �̸� (������Ʈ)
	wstring fileT;				// ���� �̸� (������)

	// ���۰� ����
	Matrix fixedView;			// ���� ī�޶�
	Matrix fixedProj;			// ���� ī�޶�
	float scroll;				// Ȯ�� / ���
	Vector2 mouseLog;			// ���콺 ���� ��ġ
	STATE state;				// ���� ���� ��Ȳ
	int phase;					// ���� �������� ��Ȳ
	char* phaseMenu[2];			// ���� ��Ȳ ����Ʈ
	int partSelect;				// ���� ���õ� �̹���
	DataSet* dataSelect;		// ���� ���õ� �̹����� ������
	int animSelect;				// ���� ���õ� �ִϸ��̼�
	float timeSelect;			// ���� ���õ� �ð�
	float timePlay;				// ����ð�
	bool isPlay;				// ���������

	// �ε� ��������
	char buf[20];				// ��Ʈ�� ������ �ε带 ���� ����
	
	// Data.txt
	int partNum;				// �̹��� ����
	vector<Sprite*> parts;		// ���� �̹���
	vector<DataSet*> partDatas;	// �̹��� �ʱ� ������

	// Anim.txt
	int animNum;				// �ִϸ��̼� ����
	vector<AnimData*> anims;	// �ִϸ��̼� ������

	// UI
	vector<class Line*> centerLine;		// �߾Ӽ�
	class Line* direction;				// �� �̹��� ����
	Sprite* tableBackground;			// Ÿ�����̺� ���
	vector<class Line*> timeHLines;		// Ÿ�����̺� ���� (���μ�)
	vector<class Line*> timeVLines;		// Ÿ�����̺� ���� (����)
	class Line* timeLineMax;			// Ÿ�����̺� ���� (���� �ð�)
	class Line* timeLineSelect;			// Ÿ�����̺� ���� (���� �ð�)

	//vector<class Line*> TempLines;
};



