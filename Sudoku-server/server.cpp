#include<io.h>
#include"CPlayer.h"
#include "generator.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
extern CPlayer player[1000];
extern CRoom rooms[10000];
extern MsgList msglist;
SOCKET serverSocket;
void Write_Grade(int player_ID, int best_consume, int best_difficulty, int best_score); //����player_ID����ҵĳɼ�д����������ݿ⣬ע��ÿ����һ�κ�����˳��ʹ����ҵ�Num_Competition��һ
void CreateRoom(int person,int room);
void Join(int person,int room); //��person�����״̬��Ϊ�ȴ���Ϸ��ʼ
void Send_Rank(int dest); //����GPA������ǰʮλ��player[dest]
void Log(char* name, char* passwd, int ID); //palyer[ID]��¼
void Reg(char* name, char* passwd, int ID); //player[ID]ע�ᣬ֮ǰ�����οͣ����ܲ�����ʽ��Ϸ
void Send_Sudoku(int src);//ע��ֻ�ǽ���Ϣ������У���������
void Process(Msg message, int src); //������Ϣ
void Show_IP(); //��ӡIP��ַ
int state = 0;//0��ʾ�ȴ���Ҽ�����Ϸ��1��ʾ�Ѿ���ʼ��Ϸ
const int g_cnt[3] = { 10, 6, 9 };
clock_t clk;
int main()
{
	srand(time(0));
	int mode = 1;
	clk = clock();
	sockaddr_in addrSrv;
	WSADATA wsadata;
	//��ʼ���׽��ֿ�
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(serverSocket, FIONBIO, (u_long FAR*) & mode);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8000);
	bind(serverSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	listen(serverSocket, 100);
	bool display_players;
	while (1)
	{
		display_players = (clock() - clk >= 300);
		if (display_players)
		{
			clk = clock();
			system("cls");
			Show_IP();
		}
		for (int i = 0; i < 1000; i++) //���μ��ÿ����ң����Ƿ�����
		{
			player[i].check();
			if (!player[i].connected&&player[i].connect(serverSocket)) player[i].ID=i;
		}
		Msg mes;
		if (display_players) printf("Player information above:\n");
		for (int i = 0; i < 1000; i++)
		{
			player[i].check();
			if (player[i].connected)
			{
				if (player[i].receive(&mes)) Process(mes, i); //���������Ϣ�����д���
				msglist.Send(); //����Ϣ�б��е���Ϣ����
				//������������ʾ��һ�����Ϣ
				if (display_players)
				{
					printf("player[%d].name=%s\nplayer[%d].room=%d\nplayer[%d].best_score=%d\n", i, player[i].name, i, player[i].room, i, player[i].best_score);
					switch (player[i].state)
					{
					case LOBBY:
						printf("player[%d].state=LOBBY\n", i);
						break;
					case WAITING:
						printf("player[%d].state=WAITING\n", i);
						break;
					case PLAYING:
						printf("player[%d].state=PLAYING\n", i);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	closesocket(serverSocket);
	return 0;
}

void MsgList::add(Msg message, int dest)
{
	messages.push(message);
	Dest.push(dest);
}

void MsgList::Send()
{
	while (!Dest.empty())
	{
		player[Dest.front()].check();
		if (player[Dest.front()].connected)
		{
			player[Dest.front()].send_message(&messages.front());
		}
		messages.pop();
		Dest.pop();
	}
}

void Write_Grade(int player_ID, int best_consume, int best_difficulty, int best_score)
{
	//˼·����OutFile��д���º������������ݣ���ɾ��ԭ�����ļ��������ļ�������Ϊԭ�������ļ���
	FILE* InFile;//ԭ�����ļ�
	FILE* OutFile;//�������ļ�
	if (_access("data.dat", 0) != -1)
		InFile = fopen("data.dat", "rb");
	else
		InFile = fopen("data.dat", "wb+");
	OutFile = fopen("tmp.dat", "wb");
	Player_Information buf;
	while (!feof(InFile) && fread(&buf, sizeof(buf), 1, InFile))
	{
		if (strcmp(buf.name, player[player_ID].name) != 0)//���������Ϣ�ճ�
		{
			fwrite(&buf, sizeof(buf), 1, OutFile);
			continue;
		}
		//��д�������Ϣ
		if (buf.best_score >= best_score)
		{
			fwrite(&buf, sizeof(buf), 1, OutFile);
			continue;
		}
		buf.best_difficulty = best_difficulty;
		buf.best_consume = best_consume;
		buf.best_score = best_score;
		fwrite(&buf, sizeof(buf), 1, OutFile);
	}
	fclose(InFile);
	fclose(OutFile);
	//��������ļ��滻
	remove("data.dat");//ɾ�����ļ�
	rename("tmp.dat", "data.dat");//���������ļ�
}

void CreateRoom(int person,int room)
{
	if (rooms[room].occupied)
	{
		Msg mes;
		mes.ID = CREATE;
		mes.num[0] = 0;
		msglist.add(mes, person);
	}
	else
	{
		player[person].is_host = 1;
		player[person].state = WAITING;
		player[person].room = room;
		Msg mes;
		mes.ID = CREATE;
		mes.num[0] = 1;
		msglist.add(mes, person);
		rooms[player[person].room].host = player[person];
		rooms[player[person].room].occupied = 1;
		rooms[player[person].room].m_players.push_back(player[person]);
	}
}

void Join(int person,int room)
{
	if (rooms[room].occupied&&rooms[room].m_players[0].state!=PLAYING)
	{
		player[person].is_host = 0;
		player[person].state = WAITING;
		player[person].room = room;
		Msg mes;
		mes.ID = JOIN;
		mes.num[0] = 1;
		strcpy(mes.string1, rooms[player[person].room].host.name);
		msglist.add(mes, person);
		rooms[player[person].room].m_players.push_back(player[person]);
	}
	else
	{
		Msg mes;
		mes.ID = JOIN;
		mes.num[0] = 0;
		msglist.add(mes, person);
	}
}

void Send_Rank(int dest)
{
	Msg mes;
	mes.ID = CLEAR_RANK;
	msglist.add(mes, dest);//֪ͨ�ͻ�����������б�
	Player_Information info[200]; //infoΪinformation����д����¼���ݿ�10000����ҵ���Ϣ�����Ҫ�����ݿⲻ�ܶ���200��ң�
	FILE* InFile;
	if (_access("data.dat", 0) != -1)
		InFile = fopen("data.dat", "rb");
	else
		InFile = fopen("data.dat", "wb+");
	int Num_of_Players = 0; //���ݿ���һ�����˶��������Ϣ
	while (!feof(InFile) && fread(&info[Num_of_Players], sizeof(info[Num_of_Players]), 1, InFile) && Num_of_Players < 200)
	{
		Num_of_Players++;
	}
	fclose(InFile);
	qsort(info, Num_of_Players, sizeof(Player_Information), comp); //info��GPA����
	mes.ID = RANK_ITEM;
	bool self_involved = 0;
	Msg self;
	for (int i = 0; i < Num_of_Players; i++) //ѡ������ǰʮ�����͹�ȥ
	{
		if (info[i].best_consume == 0) continue;
		//�����Լ�������
		if (strcmp(info[i].name, player[dest].name) == 0)
		{
			self_involved = 1;
			self.ID = RANK_ITEM;
			self.num[0] = info[i].best_consume;
			self.num[1] = info[i].best_difficulty;
			self.num[2] = info[i].best_score;
			strcpy(self.string1, info[i].name);
			strcpy(self.string2, std::to_string(i+1).c_str());
			msglist.add(self, dest);
			continue;
		}
		if (i >= 10 && self_involved) break;
		if (i >= 10) continue;
		//��������������
		mes.num[0] = info[i].best_consume;
		mes.num[1] = info[i].best_difficulty;
		mes.num[2] = info[i].best_score;
		strcpy(mes.string1, info[i].name);
		msglist.add(mes, dest);
	}
	return;
}

void Log(char* name, char* passwd, int ID)
{
	Msg mes;
	Player_Information info;
	bool success = false;
	FILE* InFile; //���ļ����������û���Ϣ
	if (_access("data.dat", 0) != -1)
		InFile = fopen("data.dat", "rb");
	else
		InFile = fopen("data.dat", "wb+");
	while (!feof(InFile) && fread(&info, sizeof(info), 1, InFile))
	{
		if (strcmp(info.name, name) == 0 && strcmp(info.passwd, passwd) == 0)
		{
			success = 1;
			break;
		}
	}
	fclose(InFile);
	mes.ID = LOG_RESULT;
	if (!success)//��¼ʧ��
	{
		mes.num[0] = 0;
		msglist.add(mes, ID);
		return;
	}
	//��¼�ɹ�
	mes.num[0] = 1;
	msglist.add(mes, ID);
	//��ʼ�������Ϣ
	strcpy(player[ID].name, info.name);
	player[ID].best_consume = info.best_consume;
	player[ID].best_difficulty = info.best_difficulty;
	player[ID].best_score = info.best_score;
	player[ID].is_host = 0;
	player[ID].state = LOBBY;
	return;
}

void Reg(char* name, char* passwd, int ID)
{
	Msg mes;
	Player_Information info;
	bool success = true;
	FILE* InFile;
	InFile = fopen("data.dat", "ab+");
	int Num_of_Players = 0;
	while (!feof(InFile) && fread(&info, sizeof(info), 1, InFile))
	{
		if (strcmp(info.name, name) == 0)
		{
			success = false;
			break;
		}
	}
	mes.ID = REG_RESULT;
	if (!success)//ע��ʧ��
	{
		mes.num[0] = 0;
		msglist.add(mes, ID);
		return;
	}
	mes.num[0] = 1;
	msglist.add(mes, ID);
	info.best_consume = info.best_difficulty = info.best_score = 0;
	strcpy(info.name, name);
	strcpy(info.passwd, passwd);
	fwrite(&info, sizeof(Player_Information), 1, InFile);//д��������
	fclose(InFile);
	return;
}

void Send_Sudoku(int src)
{
	Msg mes;
	mes.ID = BEGIN_GAME;
	Generator puzzle;
	puzzle.CreateSudoku();
	puzzle.calculateDifficulty();
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			mes.string1[9 * i + j] = '0'+puzzle.grid[i][j];
			mes.string2[9 * i + j] = '0' + puzzle.solnGrid[i][j];
		}
	}
	mes.num[0] = puzzle.difficultyLevel;
	mes.num[1] = rooms[player[src].room].m_players.size();
	mes.string1[81] = 0;
	mes.string2[81] = 0;
	for (int i = 0; i < rooms[player[src].room].m_players.size(); i++)
	{
		player[rooms[player[src].room].m_players[i].ID].state = PLAYING;
		rooms[player[src].room].m_players[i].state = PLAYING;
		msglist.add(mes, rooms[player[src].room].m_players[i].ID);
	}
	return;
}


void Process(Msg message, int src)
{
	Msg mes;
	switch (message.ID)
	{
	case CREATE:
		CreateRoom(src,message.num[0]);
		break;
	case JOIN:
		Join(src,message.num[0]);
		break;
	case BEGIN_GAME:
		Send_Sudoku(src);
		break;
	case END_GAME:
		//��дGPA���ı���Ҳ���
		player[src].state = LOBBY;
		Write_Grade(src, message.num[0], message.num[1], message.num[2]);
		
		if (player[src].best_score < message.num[2])
		{
			player[src].best_consume = message.num[0];
			player[src].best_difficulty = message.num[1];
			player[src].best_score = message.num[2];
		}
		//��������ҷ��ͳɼ���Ϣ
		mes.ID = CLEAR_RANK;
		msglist.add(mes, src);
		mes.ID = RANK_ITEM;
		for(int i=0;i<3;i++) mes.num[i]=message.num[i];
		strcpy(mes.string1, player[src].name);
		if (rooms[player[src].room].occupied) rooms[player[src].room].inform(mes);
		rooms[player[src].room].del(player[src]);
		player[src].is_host = 0;
		player[src].room = 0;
		break;
	case LOG:
		Log(message.string1, message.string2, src);
		break;
	case REG:
		Reg(message.string1, message.string2, src);
		break;
	case RANK_RQ:
		Send_Rank(src);
		break;
	case CHANGE:
		player[src].is_host = 0;
		rooms[player[src].room].del(player[src]);
		//������������շ���
		if (rooms[player[src].room].m_players.size() != 0)
		{
			mes.ID = CHANGE;
			strcpy(mes.string1, rooms[player[src].room].m_players[0].name);
			rooms[player[src].room].m_players[0].is_host = 1;
			player[rooms[player[src].room].m_players[0].ID].is_host = 1;
			rooms[player[src].room].host = rooms[player[src].room].m_players[0];
			rooms[player[src].room].inform(mes);
		}
		else rooms[player[src].room].occupied = 0;
		player[src].room = 0;
	break;
	case BUMP:
		player[src].last_bump_time = clock();
		break;
	default:
		break;
	}
}

void Show_IP()
{
	char ip4[20];
	// ��ñ���������
	char hostname[MAX_PATH];
	memset(hostname, 0, MAX_PATH);
	gethostname(hostname, MAX_PATH);
	struct hostent FAR* lpHostEnt = gethostbyname(hostname);
	if (lpHostEnt == NULL)
	{
		return;
	}
	// ȡ��IP��ַ�б��еĵ�һ��Ϊ���ص�IP, ����������IP(��Ϊһ̨�������ܻ�󶨶��IP)
	LPSTR lpAddr = lpHostEnt->h_addr_list[0];
	// ��IP��ַת�����ַ�����ʽ
	struct in_addr inAddr;
	memmove(&inAddr, lpAddr, 4);
	strcpy(ip4, inet_ntoa(inAddr));
	printf("IP address:%s\n", ip4);
	return;
}
