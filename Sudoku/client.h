#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
//ID of structure MSG
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
#define DISCONNECT 12
#define BUMP 13

//player state
#define LOBBY 0
#define WAITING 1 //waiting to play
#define PLAYING 2
#pragma comment(lib, "ws2_32.lib") 
struct Msg
{
	int ID;
	int num[3] = { 0 };
	char string1[82] = "?";
	char string2[82] = "?";
};

DWORD WINAPI CreateCilent(LPVOID);
bool SendData(const char* buf, int buf_length);
int send_message(Msg* message);//����message��Ϣ�������
void Send(); //�����б���������Ϣ

void MsgToString(Msg message, char* ans);//��Msg�����ݱ�ʾ���ַ�����ʽ���Ա㷢��
Msg StringToMsg(char* str);//���ַ��������Msg�����ݸ�ʽ
