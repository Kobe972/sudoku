#include "CPlayer.h"
CRoom rooms[10000];
CPlayer player[1000];
MsgList msglist;
bool SendData(SOCKET m_hSocket, const char* buf, int buf_length)
{
	//已发送的字节数目
	int sent_bytes = 0;
	int ret = 0;
	while (true)
	{
		ret = send(m_hSocket, buf + sent_bytes, buf_length - sent_bytes, 0);
		if (ret == -1)
		{
			if (errno == EWOULDBLOCK)
			{
				//严谨的做法，这里如果发不出去，应该缓存尚未发出去的数据，后面介绍
				break;
			}
			else if (errno == EINTR)
				continue;
			else
				return true;
		}
		else if (ret == 0)
		{
			return true;
		}

		sent_bytes += ret;
		if (sent_bytes == buf_length)
			break;

		//稍稍降低 CPU 的使用率
		Sleep(1);
	}

	return false;
}
int CPlayer::connect(SOCKET listening_sock)
{
	int len = sizeof(SOCKADDR);
	conn = accept(listening_sock, (SOCKADDR*)&clientAddr, &len);
	if (conn != INVALID_SOCKET)
	{
		last_bump_time = clock();
		connected = true;
		return 1;
	}
	return 0;
}

int CPlayer::receive(Msg* message)
{
	char buf[50];
	if (connected == false) return 0;
		if (recv(conn, buf, sizeof(buf), 0) > 0)
			{
				StringToMsg(message, buf);
				return 1;
			}
		message->ID = 20;
		message->num[0] = message->num[1] = message->num[2] = 0;
		strcpy(message->string1, "?");
		strcpy(message->string2, "?");
	return 0;
}

int CPlayer::send_message(Msg* message)
{
	char buf[300] = { 0 };
	MsgToString(*message, buf);
	if (connected == false) return 0;
	if (SendData(conn, buf, sizeof(buf))) return 1;
	return 0;
}

void MsgToString(Msg message, char* ans)
{
	std::string tmp;
	tmp = std::to_string(message.ID) + " " + std::to_string(message.num[0]) + " " + std::to_string(message.num[1]) + "  "+ std::to_string(message.num[2]) + " ";
	strcpy(ans, tmp.c_str());
	strcat(ans, message.string1);
	strcat(ans, " ");
	strcat(ans, message.string2);
	return;
}

void StringToMsg(Msg* message, char* str)
{
	int spaces = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == ' ') spaces++;
	}
	if (spaces == 6)
	{
		char* tmp;
		tmp = strtok(str, " ");
		message->ID = atoi(tmp);
		tmp = strtok(NULL, " ");
		message->num[0] = atoi(tmp);
		tmp = strtok(NULL, " ");
		message->num[1] = atoi(tmp);
		tmp = strtok(NULL, " ");
		message->num[2] = atoi(tmp);
		tmp = strtok(NULL, " ");
		strcpy(message->string1, tmp);
		tmp = strtok(NULL, " ");
		strcpy(message->string2, tmp);
	}
	else
	{
		message->ID = 20;
		message->num[0] = message->num[1] = message->num[2] = 0;
		strcpy(message->string1, "?");
		strcpy(message->string2, "?");
	}
	return;
}

void CPlayer::check()
{
	int is_ok;
	char buffer[1];
	if (!connected) return;
	/*MSG_PEEK表示从输入队列中读数据但并不将数据从输入队列中移除*/
	recv(conn, buffer, 1, MSG_PEEK);
	is_ok = (WSAECONNRESET != WSAGetLastError());
	if (clock() - last_bump_time >= 8000) is_ok = 0;
	if (!is_ok)
	{
		int mode = 1;
		conn = socket(AF_INET, SOCK_STREAM, 0);
		ioctlsocket(conn, FIONBIO, (u_long FAR*) & mode);
		best_consume = 0;
		best_difficulty = 0;
		best_score = 0;
		state = LOBBY;
		connected = false;
		if (is_host)
		{
			is_host = 0;
			rooms[room].del(*this);
			strcpy(name, "Tourist");
			//更换房主或清空房间
			if (rooms[room].m_players.size() != 0)
			{
				Msg message;
				message.ID = CHANGE;
				strcpy(message.string1, rooms[room].m_players[0].name);
				rooms[room].m_players[0].is_host = 1;
				rooms[room].host = rooms[room].m_players[0];
				rooms[room].inform(message);
			}
			else rooms[room].occupied = 0;
		}
		else
		{
			if (state == WAITING || state == PLAYING)
			{
				rooms[room].del(*this);
			}
		}
	}
}
int comp(const void* p1, const void* p2)
{
	Player_Information i1 = *(Player_Information*)p1;
	Player_Information i2 = *(Player_Information*)p2;
	if (i1.best_score > i2.best_score) return -1;
	else if (i1.best_score < i2.best_score) return 1;
	else return 0;
}

void CRoom::del(CPlayer player)
{
	vector<CPlayer>::iterator ite;
	//从房间移除玩家
	for (ite = m_players.begin(); ite < m_players.end(); ite++)
	{
		if (strcmp((*ite).name, player.name) == 0)
		{
			m_players.erase(ite);
			break;
		}
	}
	if (m_players.empty()) occupied = 0;
}

void CRoom::inform(Msg message)
{
	for (int i = 0; i < m_players.size(); i++)
	{
		msglist.add(message, m_players[i].ID);
	}
}

void CRoom::clear()
{
	occupied = 0;
	m_players.clear();
}
