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
#define SHOW_REC (EGameState::eGameStateShowRecords) //չʾ��Ϸ��¼
#define SAVE_REC (EGameState::eGameStateSaveRecords) //������Ϸ��¼
#define LOGIN (EGameState::eGameStateLogin) 
#define REGISTRY (EGameState::eGameStateReg) 
#define CONNECTTOSERVE (EGameState::eGameStateConnectToServ) //����IP�����ӷ�����
#define WAITTOCONNECT (EGameState::eGameStateWaitToConnect) //�ȴ�����
#define MULTIPLAYER (EGameState::eGameStateMultiplePlayer) //������Ϸ��ʽ��ʼ
#define MULTIEND (EGameState::eGameStateMultipleEnd) //������Ϸ������չʾ����
#define CREATE_ROOM (EGameState::eGameStateCreateRoom) //��������
#define JOIN_ROOM (EGameState::eGameStateJoin) //���뷿��
#define CHOOSE_MODE (EGameState::eGameStateChooseMode) //ѡ�񴴽����仹�Ǽ��뷿��
#define WAIT_GRID (EGameState::eGameStateWaitGrid)
#define WAIT_TO_BEGIN (EGameState::eGameStateWaitToBegin) //�ȴ���Ϸ��ʼ
#define WAIT_FOR_BEGINNING (EGameState::eGameStateWaitForBeginning) //�ȴ�������ʼ��Ϸ
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
	//GameState ��������
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
	void Login();//��¼
	void Reg();//ע���˺�
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
	bool waiting_srv = 0;//Ϊ1��ʾ����ȴ���������Ӧ
public: // serve and client
	void ProcessSerMessage();//�����ź�
	void Load_Rank(int best_consume, int best_difficulty, int best_score, char* username, int rank = 0);
private:
	bool m_loggedin;//whether logged in or not(1 indicates logged in,0 not)
	int m_connected;
	std::vector<RankItem> m_ranklist;
	std::string m_username;
};