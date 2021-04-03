#pragma once
#include"client.h"
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
#define SHOW_REC (EGameState::eGameStateShowRecords) //展示游戏纪录
#define SAVE_REC (EGameState::eGameStateSaveRecords) //保存游戏纪录
#define LOGIN (EGameState::eGameStateLogin) 
#define REGISTRY (EGameState::eGameStateReg) 
#define CONNECTTOSERVE (EGameState::eGameStateConnectToServ) //输入IP号连接服务器
#define WAITTOCONNECT (EGameState::eGameStateWaitToConnect) //等待连接
#define MULTIPLAYER (EGameState::eGameStateMultiplePlayer) //多人游戏正式开始
#define MULTIEND (EGameState::eGameStateMultipleEnd) //多人游戏结束，展示排名
#define CREATE_ROOM (EGameState::eGameStateCreateRoom) //创建房间
#define JOIN_ROOM (EGameState::eGameStateJoin) //加入房间
#define CHOOSE_MODE (EGameState::eGameStateChooseMode) //选择创建房间还是加入房间
#define WAIT_GRID (EGameState::eGameStateWaitGrid)
#define WAIT_TO_BEGIN (EGameState::eGameStateWaitToBegin) //等待游戏开始
#define WAIT_FOR_BEGINNING (EGameState::eGameStateWaitForBeginning) //等待房主开始游戏
#define SHOW_RANK (EGameState::eGameStateShowRank)

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
struct RankItem
{
	bool is_self = 0;
	char name[20];
	int rank=0;
	int best_consume;
	int best_difficulty;
	int best_score;
};

class CGame
{
public:
	void GameInit();
	void GameMain();
	~CGame();
private:
	bool g_IsSilent;
	bool is_host = 0;
	vector<RecordItem> m_RecordList;
public:
	enum EGameState {
		eGameStatePreface=0,eGameStateMainMenu = 1, eGameStateCreateSudoku, eGameStateSinglePlayer, eGameStateSingleEnd,
		eGameStateHelp,eGameStateAns, eGameStatePrepareRecordList, eGameStateShowRecords, eGameStateSaveRecords, eGameStateLogin,
		eGameStateReg, eGameStateCreateRoom, eGameStateJoin, eGameStateMultiplePlayer, eGameStateMultipleEnd, eGameStateConnectToServ,
		eGameStateWaitToConnect,eGameStateChooseMode,eGameStateWaitGrid, eGameStateWaitToBegin,
		eGameStateWaitForBeginning, eGameStateShowRank
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
	void ChooseMode();
	void SinglePlay();
	void SingleEnd();
	void JoinRoom();
	void CreateRoom();
	void MultiPlayer();
	void MultiEnd();
	void Login();//登录
	void Reg();//注册账号
	void ShowAnswer();
	void ShowRecords();
	void Help();
	void PrepareRecordList();
	bool ButtonReturn();
	void ConnectToServ();
	void WaitGrid();
	void WaitToConnect();
	void WaitToBegin();
	void WaitForBeginning();
	void ShowRank();
private:
	HWND m_hWnd;
	CSudoku Sudoku;
	bool waiting_srv = 0;//为1表示程序等待服务器响应
public: // serve and client
	void ProcessSerMessage();//处理信号
	void Load_Rank(int best_consume, int best_difficulty, int best_score, char* username, int rank = 0);
private:
	bool m_loggedin;//whether logged in or not(1 indicates logged in,0 not)
	int m_connected;
	std::vector<RankItem> m_ranklist;
	std::string m_username;
};