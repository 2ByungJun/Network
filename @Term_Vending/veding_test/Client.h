#pragma once
#define BUFSIZE 1024
#define PORT 3000
#define IP "127.0.0.1"
// �ش����� ����
#include <winsock2.h>
#include <string>
using namespace std;
// ws2_32.lib ��ũ
#pragma comment(lib, "ws2_32.lib")
struct DATA {
	string ip;
	char Name[5][20]; // ����� �̸�
	int Stock[5]; // ����� ���
	int Sale_month[13]; // ���� ����
	int Sale_day[13][32]; // �Ϻ� ����
	int eco; // eco���� Ȯ�� ���ִ� üũ ����
};
class socketClient
{
private:
	WSADATA wsaData;
	SOCKET hSocket;
	int strLen;
	SOCKADDR_IN servAddr;
public:
	DATA data;
	char message[BUFSIZE];
	int StartClient();
	void GetIpAddress(); // client�� ip�� ������ �Լ�
};
int socketClient::StartClient()
{
	// ���� �ʱ�ȭ (������ 0, ���н� ���� �ڵ帮��)
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//ErrorHandling("WSAStartup() error!");
	}
	// ���� ���� (������ �ڵ���, ���н� "INVALID_SOCKET" ��ȯ)
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	// ���� ���� ���� ó��
	if (hSocket == INVALID_SOCKET) {
		//ErrorHandling("socket() error");
	}
	// �Ÿ� �ʱ�ȭ
	memset(&servAddr, 0, sizeof(servAddr));
	// ���� ����� ���� �⺻ ����
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(PORT);
	// ���� Ŀ��Ʈ
	if (connect(hSocket, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		//ErrorHandling("connect() error!");
	}
	GetIpAddress();
	// �޽�������
	send(hSocket, (char*)& data, sizeof(data), 0);
	// �޽��� ����
	strLen = recv(hSocket, (char*)& data, sizeof(data), 0); /* �޽��� ���� */
	// ���� ���� ��ȣ����
	closesocket(hSocket);
	// �Ҵ� ���� ���ҽ� ��ȯ.
	WSACleanup();
	return 0;
}
void socketClient::GetIpAddress()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	string ip;
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);
	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr*) * hostinfo->h_addr_list);
				data.ip = ip;
			}
		}
		WSACleanup();
	}
}