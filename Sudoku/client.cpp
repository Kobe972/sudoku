#include "client.h"

SOCKET g_connSocket;
std::queue<Msg>g_Send, g_Recv; //队列中的消息

bool SendData(const char* buf, int buf_length)
{
	int sent_bytes = 0;
	int ret = 0;
	while (true)
	{
		ret = send(g_connSocket, buf + sent_bytes, buf_length - sent_bytes, 0);
		if (ret == -1)
		{
			if (errno == EWOULDBLOCK)
			{
				break;
			}
			else if (errno == EINTR)
				continue;
			else
				return false;
		}
		else if (ret == 0)
		{
			return false;
		}

		sent_bytes += ret;
		if (sent_bytes == buf_length)
			break;
		Sleep(1);
	}

	return true;
}


DWORD WINAPI CreateCilent(LPVOID)
{
	char buf[300];
	clock_t last_bump_time = clock();
	while (1)
	{
		if (clock() - last_bump_time >= 2000)
		{
			last_bump_time = clock();
			Msg Tmesssage;
			Tmesssage.ID = BUMP;
			g_Send.push(Tmesssage);
		}
		Send();
		if (recv(g_connSocket, buf, sizeof(buf), 0) > 0)
			g_Recv.push(StringToMsg(buf));
		WSASetLastError(0);
		recv(g_connSocket, buf, 1, MSG_PEEK);
		bool is_ok = (WSAECONNRESET != WSAGetLastError());
		if (!is_ok)
		{
			Msg Tmessage;
			Tmessage.ID = DISCONNECT;
			g_Recv.push(Tmessage);
			break;
		}
	}
	//system("pause");
	return 0;
}

int send_message(Msg* message)
{
	char buf[300];
	MsgToString(*message, buf);
	if (SendData(buf, strlen(buf)+1)) return 1;
	return 0;
}

void Send()
{
	while (!g_Send.empty())
	{
		send_message(&g_Send.front());
		g_Send.pop();
	}
}

void MsgToString(Msg message, char* ans)
{
	std::string tmp;
	tmp = std::to_string(message.ID) + " " + std::to_string(message.num[0]) + " " + std::to_string(message.num[1]) + "  " + std::to_string(message.num[2]) + " ";
	strcpy(ans, tmp.c_str());
	strcat(ans, message.string1);
	strcat(ans, " ");
	strcat(ans, message.string2);
	return;
}

Msg StringToMsg(char* str)
{
	Msg message;
	char* tmp;
	tmp = strtok(str, " ");
	message.ID = atoi(tmp);
	tmp = strtok(NULL, " ");
	message.num[0] = atoi(tmp);
	tmp = strtok(NULL, " ");
	message.num[1] = atoi(tmp);
	tmp = strtok(NULL, " ");
	message.num[2] = atoi(tmp);
	tmp = strtok(NULL, " ");
	strcpy(message.string1, tmp);
	tmp = strtok(NULL, " ");
	strcpy(message.string2, tmp);
	return message;
}
