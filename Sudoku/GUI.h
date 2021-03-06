#pragma once
#include "GameEntry.h"
#include"BobClass.h"
#include<vector>
using namespace std;
//button ID
#define ISINGLE_MODE 0
#define IHELP 1
#define IRETURN 2
#define IANSWER 3
#define IREC 4
#define IRANK 5
#define IOK 6
#define IMULTI_MODE 7
#define ILOGIN 8
#define IREG 9
#define ICREATE 10
#define IJOIN 11
#define IBEGIN 12
#define ICANDIDATE 13


//button states
#define BSTATENORMAL 0
#define BSTATEON 1
#define BSTATEDOWN 2
#define BSTATEUP 3

//checkbox ID
#define J_SILENCE 0

//checkbox states
#define CSTATEON 0
#define CSTATEOFF 1

//inputbox ID
#define IIPBOX 0
#define IMATH_ANSBOX 1//used in math test
#define IPASSWORD 2
#define IROOMBOX 3
class CButton : CGameEntry
{
public:
	~CButton();
	void Create(int ID, int width, int height, int x, int y, std::string filename);
	RECT boarder; //按钮边界
	int m_Width, m_Height;
	char* m_Style[3];//各状态时的图标地址（平时、鼠标移过、按下）
	LPDIRECTDRAWSURFACE7 m_ButtonSur[3];
	BITMAP_FILE m_bitmap[3];
	int m_ID;
	int m_state;
	bool m_clipped = false;//之前鼠标是否按下
public:
	void Draw();
	void Check();//process its reaction toward mouse
};

class CCheckBox : CGameEntry
{
public:
	~CCheckBox();
	void Create(int ID, int width, int height, int x, int y, std::string filename, bool state);
	RECT boarder; //按钮边界
	int m_Width, m_Height;
	char* m_Style[2];
	LPDIRECTDRAWSURFACE7 m_CheckBoxSur[2];
	BITMAP_FILE m_bitmap[2];
	int m_ID;
	int m_state;
	int m_ClipTime = GetTickCount(); //last mouse clip time
public:
	void Draw();
	void Check();//process its reaction toward mouse
};

class CInputBox : CGameEntry
{
public:
	void Create(int ID, int width, int height, int x, int y, bool password);
	void Load(char* buf);
	bool m_password;
	RECT boarder; //边界
	int m_Width, m_Height;
	bool m_activated = false;
	char m_input[50];
	int m_ID;
	vector<int> last_inputs;
	int next_available[256];
public:
	void Draw();
	void Check();
};

class CFont
{
public:
	CFont();
	HDC hdc;
	HFONT hf;
	LOGFONT lf;
	void SetType(int height, int width, int weight);
	void Uself();
};
