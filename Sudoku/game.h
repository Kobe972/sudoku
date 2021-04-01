#pragma once
#include"CSudoku.h"
#include <Windows.h>
#include<direct.h>
#include<math.h>
//then define game states
#define PREFACE (EGameState::eGameStatePreface)
#define MAINMENU (EGameState::eGameStateMainMenu)
#define CREATE_SU (EGameState::eGameStateCreateSudoku)
#define SINGLE_PLAY (EGameState::eGameStateSinglePlayer)
#define SINGLE_END (EGameState::eGameStateSingleEnd)
#define HELP (EGameState::eGameStateHelp)
#define ANSWER (EGameState::eGameStateAns)
#define PRE_REC (EGameState::eGameStatePrepareRecordList)
#define SHOW_REC (EGameState::eGameStateShowRecords)
#define SAVE_REC (EGameState::eGameStateSaveRecords)

#pragma warning
EXTERN_BOB_OBJECTS()
EXTERN_INPUT_DATA()
string GetRandomBGMusic();
int comp(const void* p1, const void* p2);
struct RecordItem
{
	int T_Consuming;
	int difficulty;
	int global;
};

class CGame
{
public:
	void GameInit();
	void GameMain();
	~CGame();
private:
	bool m_state;
	bool g_IsSilent;
	vector<RecordItem> m_RecordList;
public:
	enum EGameState {
		eGameStatePreface=0,eGameStateMainMenu = 1, eGameStateCreateSudoku, eGameStateSinglePlayer, eGameStateSingleEnd,
		eGameStateHelp,eGameStateAns, eGameStatePrepareRecordList, eGameStateShowRecords, eGameStateSaveRecords
	};
	//GameState 望文生义
private:
	EGameState m_eGameState{ CGame::EGameState::eGameStateMainMenu };
public:
	inline void SetGameState(EGameState eGameStateCurrent);
	inline void SetWindowHandle(HWND hwnd);
	void GetCurMsg();
	void ProcessKeyMsg();
	void ProcessButtonMsg();
	void ProcessCheckBoxMsg();
	void Preface();
	void ShowMenu();
	void Create_Sudoku();
	void SinglePlay();
	void SingleEnd();
	void ShowAnswer();
	void ShowRecords();
	void Help();
	void PrepareRecordList();
	bool ButtonReturn();
private:
	HWND m_hWnd;
	CSudoku Sudoku;
};