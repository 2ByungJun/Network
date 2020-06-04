#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include <ctime>
#include "Client.h"
using namespace std;
typedef struct Node {
	int check;
	struct Node* next;
}Node;
Node* Create() { // Linked List 노드 생성
	Node* NewNode = new Node;
	NewNode->check = 1;
	NewNode->next = NULL;
	return NewNode;
}
void Insert(Node** Head, Node* NewNode) { // Linked List에 노드 삽입
	if (*Head == NULL) {
		*Head = NewNode;
	}
	else {
		Node* Current = *Head;
		while (Current->next != NULL) {
			Current = Current->next;
		}
		Current->next = NewNode;
	}
}
void Delete(Node*& Head) { // Linked List 노드 삭제
	Head = Head->next;
}
int Count(Node* Head) { // Linked List 노드 갯수 세기
	int cnt = 0;
	Node* Current = Head;
	while (Current != NULL) {
		cnt++;
		Current = Current->next;
	}
	return cnt;
}
class Machine {
private:
	char Name[5][20]; // 음료수 이름
	int Cost[5]; // 음료수 가격
	Node* Stock[5]; // 음료수 재고
	// 자판기의 거스름 돈
	int Coin_10;
	int Coin_50;
	int Coin_100;
	int Coin_500;
	int Coin_1000;
	int Coin_Sum; // 총 입력받은 돈
	int count_1000; // 지폐의 상한선 3000원을 위해 만든 카운트 변수
	int Sale_month[13]; // 월별 매출
	int Sale_day[13][32]; // 일별 매출
	int Sale_drink[5][13][32]; // 음료수별 일별 매출
	int Sold_out[5][13][32]; // 음료수별 매진 날짜
	int month, day; // 현재 날짜
	struct tm A; // 현재 날짜를 구하기 위한 변수
	time_t t;
	socketClient client;
public:
	Machine(); // 생성자
	~Machine(); // 소멸자
	void Coin_Input(); // 동전 입력
	void Sell(int*); // 음료수 판매
	void Change(int*); // 거스름돈
	void Menu(); // 메뉴
	void Admin_Menu(); // 관리자메뉴
	void Sales(); // 매출
	void Modify_Name(); // 음료수 이름 수정
	void Modify_Cost(); // 음료수 가격 수정
	void Modify_Stock(); // 음료수 재고 확인, 보충
	void Collect_Money(); // 수금
	void Sold_Out(); // 음료수 매진 날짜 확인
	void Save(); // 저장
	void Load(); // 불러오기
};
Machine::Machine() {
	Node* NewNode;
	// 처음 모든 음료의 재고는 3개
	for (int k = 0; k < 5; k++) {
		Stock[k] = NULL;
		for (int i = 0; i < 3; i++) {
			NewNode = Create();
			Insert(&Stock[k], NewNode);
		}
	}
	// 음료의 이름들을 저장
	strcpy(Name[0], "물");
	strcpy(Name[1], "커피");
	strcpy(Name[2], "이온음료");
	strcpy(Name[3], "고급커피");
	strcpy(Name[4], "탄산음료");
	// 음료의 가격들을 저장
	Cost[0] = 450;
	Cost[1] = 500;
	Cost[2] = 550;
	Cost[3] = 700;
	Cost[4] = 750;
	// 자판기의 거스름돈 초기화
	Coin_10 = 5;
	Coin_50 = 5;
	Coin_100 = 5;
	Coin_500 = 5;
	Coin_1000 = 5;
	count_1000 = 0;
	// 입력된 돈의 합 초기화
	Coin_Sum = 0;
	// 일별, 월별 매출 초기화
	for (int i = 0; i < 13; i++) {
		Sale_month[i] = 0;
		for (int j = 0; j < 32; j++) {
			Sale_day[i][j] = 0;
			for (int k = 0; k < 5; k++) {
				Sale_drink[k][i][j] = 0;
			}
		}
	}
	// 현재 날짜 구하기
	time(&t);
	localtime_s(&A, &t);
	month = A.tm_mon + 1;
	day = A.tm_mday;
	Load(); // 불러오기함수 호출
	for (int i = 0; i < 5; i++) {
		strcpy(client.data.Name[i], Name[i]);
		client.data.Stock[i] = Count(Stock[i]);
	}
	for (int i = 0; i < 13; i++) {
		client.data.Sale_month[i] = Sale_month[i];
		for (int j = 0; j < 32; j++) {
			client.data.Sale_day[i][j] = Sale_day[i][j];
		}
	}
	client.StartClient();
	if (client.data.eco == 1) { // 이미 예전에 접속한적이 있어서 eco를 받았다면
		for (int i = 0; i < 5; i++) { // 서버에서 수정된 이름들을 읽어와 변경해준다.
			strcpy(Name[i], client.data.Name[i]);
		}
	}
}
Machine::~Machine() {
	Save();
}
void Machine::Save() {
	ofstream out("data.txt");
	// 음료수 이름 저장
	for (int i = 0; i < 5; i++)
		out << Name[i] << endl;
	// 음료수 가격 저장
	for (int i = 0; i < 5; i++)
		out << Cost[i] << endl;
	// 음료수 재고 저장
	for (int i = 0; i < 5; i++) {
		out << Count(Stock[i]) << endl;
	}
	// 자판기의 거스름 돈 저장
	out << Coin_10 << endl;
	out << Coin_50 << endl;
	out << Coin_100 << endl;
	out << Coin_500 << endl;
	out << Coin_1000 << endl;
	// 월별 매출 저장
	for (int i = 1; i <= 12; i++)
		out << Sale_month[i] << " ";
	// 일별 매출 저장
	for (int i = 1; i <= 12; i++)
		for (int j = 1; j <= 31; j++)
			out << Sale_day[i][j] << " ";
	// 음료수별 일별 매출 저장
	for (int k = 0; k < 5; k++)
		for (int i = 1; i <= 12; i++)
			for (int j = 1; j <= 31; j++)
				out << Sale_drink[k][i][j] << " ";
}
void Machine::Load() {
	ifstream in("data.txt");
	if (in.eof() || !in.is_open())return; // 파일이 없거나 내용이 없으면 끝내기
	// 음료수 이름 불러오기
	for (int i = 0; i < 5; i++)
		in >> Name[i];
	// 음료수 가격 불러오기
	for (int i = 0; i < 5; i++)
		in >> Cost[i];
	// 음료수 재고 불러오기
	int stock;
	Node * NewNode;
	for (int i = 0; i < 5; i++) {
		in >> stock;
		Stock[i] = NULL;
		for (int j = 0; j < stock; j++) {
			NewNode = Create();
			Insert(&Stock[i], NewNode);
		}
	}
	// 자판기의 거스름 돈 불러오기
	in >> Coin_10;
	in >> Coin_50;
	in >> Coin_100;
	in >> Coin_500;
	in >> Coin_1000;
	// 월별 매출 불러오기
	for (int i = 1; i <= 12; i++)
		in >> Sale_month[i];
	// 일별 매출 불러오기
	for (int i = 1; i <= 12; i++)
		for (int j = 1; j <= 31; j++)
			in >> Sale_day[i][j];
	// 음료수별 일별 매출 불러오기
	for (int k = 0; k < 5; k++)
		for (int i = 1; i <= 12; i++)
			for (int j = 1; j <= 31; j++)
				in >> Sale_drink[k][i][j];
	// 입력된 돈의 합 초기화
	Coin_Sum = 0;
	count_1000 = 0;
}
void Machine::Coin_Input() {
	int* Coin_Check;
	char Input_Temp[10];
	cout << "입력하실 화폐의 단위를 입력하세요. (10원, 50원, 100원, 500원, 1000원) ";
	Coin_Check = new int; // 입력되는 동전변수를 동적할당 한다.
	try {
		cin >> Input_Temp;
		*Coin_Check = atoi(Input_Temp);
		switch (*Coin_Check) {
		case 1000:
			count_1000++;
			if (count_1000 > 3) {
				cout << "지폐의 상한선은 3000원까지 입니다." << endl;
			}
			else if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_1000 = Coin_1000 + 1; // 자판기의 1000원 보관 변수에 더하여 저장
				Coin_Sum += *Coin_Check;
			}
			else { // 입력이 제대로 되지 않았을 경우
				throw 'e';
			}
			break;
		case 500:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_500 = Coin_500 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // 입력이 제대로 되지 않았을 경우
				throw 'e';
			}
			break;
		case 100:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_100 = Coin_100 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // 입력이 제대로 되지 않았을 경우
				throw 'e';
			}
			break;
		case 50:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_50 = Coin_50 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // 입력이 제대로 되지 않았을 경우
				throw 'e';
			}
			break;
		case 10:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_10 = Coin_10 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // 입력이 제대로 되지 않았을 경우
				throw 'e';
			}
			break;
		default:
			throw 'e';
		}
	}
	catch (char e) {
		cout << "입력에 오류가 있습니다." << endl;
	}
	free(Coin_Check); // 동전 입력이 끝나면 동적할당을 해제한다.
}
void Machine::Sell(int* money) {
	// 음료수 메뉴 출력
	cout << "------------------------------" << endl;
	for (int i = 0; i < 5; i++) {
		cout << i << ". " << setw(10) << Name[i] << " (" << Cost[i] << ")";
		if (*money >= Cost[i] && Count(Stock[i]) != 0) cout << setw(20) << "사용할 수 있음." << endl;
		else {
			if (Count(Stock[i]) == 0) cout << setw(20) << "매진" << endl;
			else cout << setw(20) << "금액이 모자람" << endl;
		}
	}
	///////////////////////////////////////////////////////////////
	cout << " 메뉴를 입력하세요. ";
	int Choice;
	cin >> Choice;
	if (*money >= Cost[Choice] && Count(Stock[Choice]) != 0) {
		cout << "당신은 " << Name[Choice] << "을(를) 얻었습니다." << endl;
		Delete(Stock[Choice]);
		*money -= Cost[Choice];
		Sale_month[month] += Cost[Choice];
		Sale_day[month][day] += Cost[Choice];
		Sale_drink[Choice][month][day] += Cost[Choice];
		if (Count(Stock[Choice]) == 0) {
			Sold_out[Choice][month][day] = 1;
		}
	}
	else {
		if (Count(Stock[Choice]) == 0) cout << "매진되었습니다." << endl;
		else cout << "금액이 모자랍니다." << endl;
	}
}
void Machine::Change(int* money) {
	int c1000, c500, c100, c50, c10;
	c1000 = c500 = c100 = c50 = c10 = 0; // 거스름 동전 카운트
	while (*money != 0) { // 삽입된 돈이 0이 될때까지
		if (*money >= 1000 && Coin_1000 > 0) { // 1000원 이상이 있고 1000원 짜리로 거슬러 줄 수 있을경우
				* money -= 1000;
			Coin_1000--;
			c1000++;
		}
		else if (*money >= 500 && Coin_500 > 0) {
			*money -= 500;
			Coin_500--;
			c500++;
		}
		else if (*money >= 100 && Coin_100 > 0) {
			*money -= 100;
			Coin_100--;
			c100++;
		}
		else if (*money >= 50 && Coin_50 > 0) {
			*money -= 50;
			Coin_50--;
			c50++;
		}
		else if (*money >= 10 && Coin_10 > 0) {
			*money -= 10;
			Coin_10--;
			c10++;
		}
		else { // 거스름돈이 없을경우?
			cout << "거스름돈이 없습니다." << endl;
		}
	}
	if (c1000 != 0)
		cout << "1000원 지폐 : " << c1000 << "장" << endl;
	if (c500 != 0)
		cout << "500원 동전 : " << c500 << "개" << endl;
	if (c100 != 0)
		cout << "100원 동전 : " << c100 << "개" << endl;
	if (c50 != 0)
		cout << "50원 동전 : " << c50 << "개" << endl;
	if (c10 != 0)
		cout << "10원 동전 : " << c10 << "개" << endl;
	cout << "반환 되었습니다." << endl;
	count_1000 = 0; // 천원 짜리 카운트를 초기화
}
void Machine::Menu() {
	int Choice;
	char Input_Temp[10];
	for (;;) {
		cout << " " << month << "/" << day << endl;
		cout << "-------------Menu------------" << endl;
		cout << " 1. 동전 삽입 " << endl;
		cout << " 2. 음료수 구매" << endl;
		cout << " 3. 거스름돈 반환" << endl;
		cout << " 4. 관리자 메뉴" << endl;
		cout << " 0. 종료" << endl;
		cout << "-----------------------------" << endl;
		cout << " 현재 삽입된 금액 : " << Coin_Sum << endl;
		cout << " 메뉴를 입력해주세요. ";
		try {
			cin >> Input_Temp;
			Choice = atoi(Input_Temp);
			switch (Choice) {
			case 1:
				Coin_Input();
				break;
			case 2:
				Sell(&Coin_Sum);
				break;
			case 3:
				Change(&Coin_Sum);
				break;
			case 4:
				system("CLS");
				Admin_Menu();
				break;
			case 0:
				if (strcmp(Input_Temp, "0") == 0)
					return;
				else {
					throw 'e';
				}
			}
		}
		catch (char e) {
			cout << "입력에 오류가 있습니다." << endl;
		}
		cout << "아무 키를 입력해주세요." << endl;
		char c = getch();
		system("CLS");
	}
}
void Machine::Admin_Menu() {
	char password[5] = { "1234" };
	char Input_Temp[10];
	char input_pass[5] = { 0 };
	int Choice;
	cout << "비밀번호를 입력해주세요 : ";
	for (int i = 0; i < 4; i++) {
		input_pass[i] = getch();
		cout << "*";
	}
	cout << endl;
	if (strcmp(password, input_pass) != 0) {
		cout << "비밀번호가 틀렸습니다." << endl;
		return;
	}
	for (;;) {
		cout << "-------------Menu------------" << endl;
		cout << " 1. 매출 출력 " << endl;
		cout << " 2. 음료수 이름 수정" << endl;
		cout << " 3. 음료수 가격 수정" << endl;
		cout << " 4. 음료수 재고 추가 " << endl;
		cout << " 5. 음료수 매진 날짜 " << endl;
		cout << " 6. 수금 " << endl;
		cout << " 0. 뒤로가기" << endl;
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
				Modify_Name();
				break;
			case 3:
				Modify_Cost();
				break;
			case 4:
				Modify_Stock();
				break;
			case 5:
				Sold_Out();
				break;
			case 6:
				Collect_Money();
				break;
			case 0:
				if (strcmp(Input_Temp, "0") == 0)
					return;
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
void Machine::Modify_Name() {
	cout << "현재 음료수 이름 : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Cost[i] << "원" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "어떤 음료수를 바꾸시겠습니까? ";
	char change[30];
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "바꾸실 이름은? ";
			cin >> change;
			strcpy(Name[i], change);
			cout << "수정이 완료되었습니다." << endl;
			break;
		}
	}
}
void Machine::Modify_Cost() {
	cout << "현재 음료수 가격 : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Cost[i] << "원" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "어떤 음료수를 바꾸시겠습니까?";
	char change[30];
	int change_cost;
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "얼마로 바꾸시겠습니까?";
			cin >> change_cost;
			Cost[i] = change_cost;
			cout << "수정이 완료되었습니다." << endl;
			break;
		}
	}
}
void Machine::Collect_Money() {
	int collect, sum;
	char Input_Temp[10];
	sum = 0;
	cout << "천원짜리 지폐 " << Coin_1000 << "장 있습니다." << endl;
	cout << "몇장을 수금하시겠습니까? (최소한 5장이 남아있어야 합니다.) " << endl;
	try {
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_1000 - 5 >= collect) {
			sum += collect * 1000;
			Coin_1000 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "오백원짜리 " << Coin_500 << "개 있습니다." << endl;
		cout << "몇개를 수금하시겠습니까? (최소한 5개가 남아있어야 합니다.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_500 - 5 >= collect) {
			sum += collect * 500;
			Coin_500 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "백원짜리 " << Coin_100 << "개 있습니다." << endl;
		cout << "몇개를 수금하시겠습니까? (최소한 5개가 남아있어야 합니다.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_100 - 5 >= collect) {
			sum += collect * 100;
			Coin_100 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "오십원짜리 " << Coin_50 << "개 있습니다." << endl;
		cout << "몇개를 수금하시겠습니까? (최소한 5개가 남아있어야 합니다.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_50 - 5 >= collect) {
			sum += collect * 50;
			Coin_50 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "십원짜리 " << Coin_10 << "개 있습니다." << endl;
		cout << "몇개를 수금하시겠습니까? (최소한 5개가 남아있어야 합니다.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_10 - 5 >= collect) {
			sum += collect * 10;
			Coin_10 -= collect;
		}
		else {
			throw 'e';
		}
	}
	catch (char e) {
		cout << "입력 오류입니다." << endl;
	}
	cout << "총 수금한 금액은 : " << sum << "입니다." << endl;
}
void Machine::Sold_Out() {
	int check;
	cout << "음료수 매진 날짜 : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << " 매진 날짜 : " << endl;
		for (int j = 1; j <= 12; j++) {
			for (int k = 1; k <= 12; k++) {
				if (Sold_out[i][j][k] == 1) {
					cout << setw(8) << j << "월 " << k << "일" << endl;
				}
			}
		}
		cout << "------------------------------" << endl;
	}
}
void Machine::Modify_Stock() {
	cout << "현재 음료수 재고 " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Count(Stock[i]) << "개" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "어떤 음료수의 재고를 추가하시겠습니까? ";
	char change[30];
	int add;
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "몇개를 더 추가하시겠습니까?";
			cin >> add;
			if (add > 0) {
				Node* NewNode;
				for (int j = 0; j < add; j++) {
					NewNode = Create();
					Insert(&Stock[i], NewNode);
				}
				cout << "추가가 완료되었습니다." << endl;
			}
			else {
				cout << "입력 에러입니다." << endl;
			}
			break;
		}
	}
}
void Machine::Sales() { // 매출 출력
	int i, j;
	// 월별 매출
	for (i = 1; i <= 12; i++) {
		cout << i << "월 매출 : " << Sale_month[i] << endl;
	}
	int check;
	// 일별 매출
	for (i = 1; i <= 12; i++) {
		check = 0;
		for (j = 1; j <= 31; j++) {
			if (Sale_day[i][j] != 0) {
				cout << i << "월 " << j << "일 매출 : " << Sale_day[i][j] << endl;
				check = 1;
			}
		}
		if (check == 1) {
			cout << "----------------------------------" << endl;
		}
	}
}
void main() {
	Machine Machine;
	Machine.Menu();
}