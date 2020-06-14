#pragma once
#define BUFSIZE 1024
#define PORT 3000
#define IP "127.0.0.1"
// 해더파일 선언
#include <winsock2.h>
#include <string>
using namespace std;
// ws2_32.lib 링크
#pragma comment(lib, "ws2_32.lib")
struct DATA {
	string ip;
	char Name[5][20]; // 음료수 이름
	int Stock[5]; // 음료수 재고
	int Sale_month[13]; // 월별 매출
	int Sale_day[13][32]; // 일별 매출
	int eco; // eco인지 확인 해주는 체크 변수
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
	void GetIpAddress(); // client의 ip를 얻어오는 함수
};
int socketClient::StartClient()
{
	// 윈속 초기화 (성공시 0, 실패시 에러 코드리턴)
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//ErrorHandling("WSAStartup() error!");
	}
	// 소켓 생성 (성공시 핸들을, 실패시 "INVALID_SOCKET" 반환)
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	// 소켓 생성 실패 처리
	if (hSocket == INVALID_SOCKET) {
		//ErrorHandling("socket() error");
	}
	// 매모리 초기화
	memset(&servAddr, 0, sizeof(servAddr));
	// 소켓 통신을 위한 기본 정보
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(PORT);
	// 서버 커넥트
	if (connect(hSocket, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		//ErrorHandling("connect() error!");
	}
	GetIpAddress();
	// 메시지전송
	send(hSocket, (char*)& data, sizeof(data), 0);
	// 메시지 수신
	strLen = recv(hSocket, (char*)& data, sizeof(data), 0); /* 메시지 수신 */
	// 소켓 종류 신호전송
	closesocket(hSocket);
	// 할당 받은 리소스 반환.
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