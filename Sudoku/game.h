#pragma once
#include"CSudoku.h"
#include <Windows.h>
#include<math.h>
//then define game states
#define PREFACE (EGameState::eGameStatePreface)
#define MAINMENU (EGameState::eGameStateMainMenu)
#define CREATE_SU (EGameState::eGameStateCreateSudoku)
#define SINGLE_PLAY (EGameState::eGameStateSinglePlayer)
#define SINGLE_END (EGameState::eGameStateSingleEnd)
#define HELP (EGameState::eGameStateHelp)

#pragma warning
EXTERN_BOB_OBJECTS()
EXTERN_INPUT_DATA()

class CGame
{
public:
	void GameInit();
	void GameMain();
	~CGame();
private:
	bool m_state;
public:
	enum EGameState {
		eGameStatePreface=0,eGameStateMainMenu = 1, eGameStateCreateSudoku, eGameStateSinglePlayer, eGameStateSingleEnd,
		eGameStateHelp
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
	void Preface();
	void ShowMenu();
	void Create_Sudoku();
	void SinglePlay();
	void Help();
	bool ButtonReturn();
private:
	HWND m_hWnd;
	CSudoku Sudoku;
};