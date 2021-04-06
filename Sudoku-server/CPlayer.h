#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
#pragma comment(lib,"User32.lib")
using namespace std;
//ID of structure MSG
#define CREATE 0//�������䣨�ͻ��˸���������
#define JOIN 1 //���뷿��,���������͸��ͻ���ʱ��num[0]Ϊ1��ʾ�ɹ�
#define END_GAME 2 //��Ϸ�������ͻ��˷���������
#define RANK_ITEM 3 //��ʾ����Ϣ�������б��е�һ����������ͻ��ˣ�
#define CLEAR_RANK 4 //��������б����������ͻ��ˣ�
//˵��һ�£��ͻ��˺���һ�����ݽṹ�����������б������е���������Ϸ�ɼ������յ�RANK_ITEM�͵����ݺ����б����������Ϣ���յ�CLEAR_RANK������б�����Ҫ��ʾ����ʱ��ֱ�Ӵ�ӡ���б���
#define LOG 5 //��¼���ͻ��˸���������
#define LOG_RESULT 6 //��¼��������������ͻ��ˣ���num=0��ʾʧ�ܣ�num=1��ʾ�ɹ���num�ĺ����Msg�ṹ�Ķ���
#define REG 7 //ע�ᣨ�ͻ��˸���������
#define REG_RESULT 8 //ע���������������ͻ��ˣ�
#define RANK_RQ 9 //�����������б��ͻ��˸���������
#define BEGIN_GAME 10 //������ʼ��Ϸ��˫��
#define CHANGE 11//��Ϸ��ʼǰ�����뿪�������Զ�����
#define MEANINGLESS 12 //������Ϣ����������Ϣת�������

//player state
#define LOBBY 0
#define WAITING 1 //waiting to play
#define PLAYING 2
struct Msg
{
	int ID = 0;
	int num[3] = { 0 };
	char string1[82] = "?";
	char string2[82] = "?";
};
class MsgList
{
public:
	queue<Msg>messages; //�����е���Ϣ
	queue<int> Dest; //����ϢҪ���͸��ĸ���ң���Dest=1��ʾ����player[1]
	void add(Msg message, int dest); //����Ϣ�б������Ϣ
	void Send(); //�����б���������Ϣ
};
void MsgToString(Msg message, char* ans);//��Msg�����ݱ�ʾ���ַ�����ʽ���Ա㷢��
void StringToMsg(Msg* message, char* str);//���ַ��������Msg�����ݸ�ʽ
class CPlayer
{
public:
	char name[20] = "Tourist";
	int best_consume = 0,best_difficulty=0,best_score=0;
	int ID;
	int room=0;
	bool is_host = 0;
	SOCKET conn;//���ںͿͻ��˽������׽���
	bool connected = false; //conn�Ƿ����Ͽͻ���
	sockaddr_in clientAddr; //�ͻ��˵�ַ��Ϣ��ֻ�Ǻ���������Ҫ������ʵ���ô���
	int state = LOBBY;//�����״̬��Ĭ����Ϣ
	int connect(SOCKET listening_sock);//���conn��û�пͻ��˿���������������
	int receive(Msg* message);//������Ϣ������message�����ܳɹ�����1�����򷵻�0
	int send_message(Msg* message);//����message��Ϣ���ͻ���
	void check();//�ж������Ƿ�Ͽ�����Ͽ�connected����
};
class CRoom
{
public:
	int occupied = 0;
	vector<CPlayer> m_players;
	CPlayer host;
	void del(CPlayer player);
	void inform(Msg message);
	void clear();
};
struct Player_Information //�����Ϣ�����ݿ��д�����ݸ�ʽ
{
	char name[20];
	char passwd[20];
	int best_consume; //�ܹ�������α���
	int best_difficulty;
	int best_score;
};
int comp(const void* p1, const void* p2); //�ȽϺ�������Player_Information�����ݰ�GPA��С������
bool SendData(SOCKET m_hSocket, const char* buf, int buf_length);


