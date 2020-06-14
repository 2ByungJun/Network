//////////////////////////////////////////////////////////////////////////////////////////////////
//
// 비동기 소켓 서버
//
#define BUFSIZE 100 // 받아올 데이터 최대 크기
#define PORT 3000 // 포트번호 할당
// 해더파일 선언
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
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
class socketServer
{
private:
	// 변수 선언
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	//소켓 핸들배열 - 연결 요청이 들어올 때마다 생성되는 소켓의 핸들을 이 배열에 저장.
	SOCKET hSockArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;
	WSAEVENT hEventArray[WSA_MAXIMUM_WAIT_EVENTS]; // 이벤트 배열
	WSAEVENT newEvent;
	WSANETWORKEVENTS netEvents;
	int clntLen;
	int sockTotal;
	int index, i;
	char message[BUFSIZE];
	int Sale_month[13]; // 월별 매출
	int strLen;
	DATA data;
	void CompressSockets(SOCKET* hSockArray, int omitIndex, int total);
	void CompressEvents(WSAEVENT* hEventArray, int omitIndex, int total);
	void ErrorHandling(char* message);
	void Print();
	void Sales();
	void ModifyName();
	void Stock();
public:
	socketServer();
	int StartServer();
};
socketServer::socketServer()
{
	sockTotal = 0;
}
int socketServer::StartServer()
{
	// 윈속 초기화 (성공시 0, 실패시 에러 코드리턴)
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");
	}
	// 소켓 생성 (성공시 핸들을, 실패시 "INVALID_SOCKET" 반환)
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	// 소켓 생성 실패 처리
	if (hServSock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}
	// 소켓 통신을 위한 기본 정보
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);
	// 주소와 Port 할당 (바인드!!)
	if (bind(hServSock, (struct sockaddr*) & servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling("bind() error");
	}
	// 이벤트 발생을 확인 (성공시 0, 실패시 "SOCKET_ERROR" 반환)
	newEvent = WSACreateEvent();
	if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR) {
		ErrorHandling("WSAEventSelect() error");
	}
	// 연결 대기 요청 상태로의 진입 (신호가 들어올때까지 대기)
	if (listen(hServSock, 5) == SOCKET_ERROR) {
		ErrorHandling("listen() error");
	}
	// 서버 소켓 핸들 정보
	hSockArray[sockTotal] = hServSock;
	// 이벤트 오브젝트 핸들 정보
	hEventArray[sockTotal] = newEvent;
	// 전체 소켓수
	sockTotal++;
	// 루프
	while (1)
	{
		// 이벤트 종류 구분하기(WSAWaitForMultipleEvents)
		index = WSAWaitForMultipleEvents(sockTotal, hEventArray, FALSE,
			WSA_INFINITE, FALSE);
		index = index - WSA_WAIT_EVENT_0;
		for (i = index; i < sockTotal; i++)
		{
			index = WSAWaitForMultipleEvents(1, &hEventArray[i], TRUE, 0, FALSE);
			if ((index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT)) continue;
			else
			{
				index = i;
				WSAEnumNetworkEvents(hSockArray[index],
					hEventArray[index], &netEvents);
				// 초기 연결 요청의 경우.
				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						puts("Accept Error");
						break;
					}
					clntLen = sizeof(clntAddr);
					// 연결을 수락
					// (accept | 성공시 소켓핸들 실패시 "INVALID_SOCKET" 반환)
					hClntSock = accept(hSockArray[index], (SOCKADDR*)& clntAddr, &clntLen);
					// 이벤트 커널 오브젝트 생성(WSACreateEvent)
					newEvent = WSACreateEvent();
					// 이벤트 발생 유무 확인(WSAEventSelect)
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);
					hEventArray[sockTotal] = newEvent;
					hSockArray[sockTotal] = hClntSock;
					sockTotal++;
					printf("새로 연결된 소켓의 핸들 %d ₩n", hClntSock);
				}
				// 데이터 전송해올 경우.
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						puts("Read Error");
						break;
					}
					//
					// 서버 작업
					//
					// 데이터를 받음 (message에 받은 데이터를 담음)
					strLen = recv(hSockArray[index - WSA_WAIT_EVENT_0], (char*)& data, sizeof(data), 0);
					ifstream in(data.ip + ".txt");
					if (in) { // 예전에 접속한 적이 있어서 파일이 있다면
					// 에코(데이터를준 클라이언트에 다시 데이터쏘기)
						data.eco = 1; // eco라는 것을 체크
						for (int i = 0; i < 5; i++) { // 음료수 이름을 읽어온다
							in >> data.Name[i];
						}
					}
					else { // 파일이 없다면 촘
						data.eco = 0;
					}
					send(hSockArray[index - WSA_WAIT_EVENT_0], (char*)& data, sizeof(data), 0);
					// 화면 출력
					Print();
				}
				// 연결 종료 요청의 경우.
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						puts("Close Error");
						break;
					}
					WSACloseEvent(hEventArray[index]);
					// 소켓 종류
					closesocket(hSockArray[index]);
					printf("종료 된 소켓의 핸들 %d ₩n", hSockArray[index]);
					sockTotal--;
					// 배열 정리.
					CompressSockets(hSockArray, index, sockTotal);
					CompressEvents(hEventArray, index, sockTotal);
				}
			}//else
		}//for
	}//while
	// 할당 받은 리소스 반환.
	WSACleanup();
	return 0;
}
/************************************
/*
/* CompressSockets
/*
*/
void socketServer::CompressSockets(SOCKET* hSockArray, int omitIndex, int total)
{
	int i;
	for (i = omitIndex; i < total; i++)
	{
		hSockArray[i] = hSockArray[i + 1];
	}
}
/************************************
/*
/* CompressEvents
/*
*/
void socketServer::CompressEvents(WSAEVENT* hEventArray, int omitIndex, int total)
{
	int i;
	for (i = omitIndex; i < total; i++)
	{
		hEventArray[i] = hEventArray[i + 1];
	}
}
/************************************
/*
/* ErrorHandling
/*
*/
void socketServer::ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('₩n', stderr);
	exit(1);
}
void socketServer::Print() {
	char Input_Temp[10];
	int Choice;
	cout << data.ip << endl;
	ofstream out(data.ip + ".txt");
	for (;;) {
		cout << "-------------Menu------------" << endl;
		cout << " 1. 매출 출력 " << endl;
		cout << " 2. 음료수 이름 수정" << endl;
		cout << " 3. 음료수 재고 확인 " << endl;
		cout << " 0. 접속 종료" << endl;
		cout << "-----------------------------" << endl;
		cout << " 메뉴를 입력하세요. ";
		try {
			cin >> Input_Temp;
			Choice = atoi(Input_Temp);
			switch (Choice) {
			case 1:
				Sales();
				break;
			case 2:
				ModifyName();
				break;
			case 3:
				Stock();
				break;
			case 0:
				if (strcmp(Input_Temp, "0") == 0) {
					for (int i = 0; i < 5; i++) {
						out << data.Name[i] << endl;
					}
					return;
				}
				else
					throw 'e';
			}
		}
		catch (char e) {
			cout << "입력에 에러가 있습니다." << endl;
		}
		cout << "아무 키를 입력해주세요." << endl;
		char c = getch();
		system("CLS");
	}
}
void socketServer::Sales() {
	int i, j;
	// 월별 매출
	for (i = 1; i <= 12; i++) {
		cout << i << "월 매출 : " << data.Sale_month[i] << endl;
	}
	int check;
	// 일별 매출
	for (i = 1; i <= 12; i++) {
		check = 0;
		for (j = 1; j <= 31; j++) {
			if (data.Sale_day[i][j] != 0) {
				cout << i << "월 " << j << "일 매출 : " << data.Sale_day[i][j] << endl;
				check = 1;
			}
		}
		if (check == 1) {
			cout << "----------------------------------" << endl;
		}
	}
}
void socketServer::ModifyName() {
	cout << "현재 음료수 이름 : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << data.Name[i] << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "어떤 음료수를 바꾸시겠습니까? ";
	char change[30];
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(data.Name[i], change) == 0) {
			cout << "바꾸실 이름은? ";
			cin >> change;
			strcpy(data.Name[i], change);
			cout << "수정이 완료되었습니다." << endl;
			break;
		}
	}
}
void socketServer::Stock() {
	cout << "현재 음료수 재고 " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << data.Name[i] << setw(10) << data.Stock[i] << "개" << endl;
	}
	cout << "-----------------------------" << endl;
}
/*
/* main
/*
*/
int main() {
	// 서버 인스턴트
	socketServer server;
	// 서버시작
	server.StartServer();
}
