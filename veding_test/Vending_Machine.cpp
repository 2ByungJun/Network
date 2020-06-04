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
Node* Create() { // Linked List ��� ����
	Node* NewNode = new Node;
	NewNode->check = 1;
	NewNode->next = NULL;
	return NewNode;
}
void Insert(Node** Head, Node* NewNode) { // Linked List�� ��� ����
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
void Delete(Node*& Head) { // Linked List ��� ����
	Head = Head->next;
}
int Count(Node* Head) { // Linked List ��� ���� ����
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
	char Name[5][20]; // ����� �̸�
	int Cost[5]; // ����� ����
	Node* Stock[5]; // ����� ���
	// ���Ǳ��� �Ž��� ��
	int Coin_10;
	int Coin_50;
	int Coin_100;
	int Coin_500;
	int Coin_1000;
	int Coin_Sum; // �� �Է¹��� ��
	int count_1000; // ������ ���Ѽ� 3000���� ���� ���� ī��Ʈ ����
	int Sale_month[13]; // ���� ����
	int Sale_day[13][32]; // �Ϻ� ����
	int Sale_drink[5][13][32]; // ������� �Ϻ� ����
	int Sold_out[5][13][32]; // ������� ���� ��¥
	int month, day; // ���� ��¥
	struct tm A; // ���� ��¥�� ���ϱ� ���� ����
	time_t t;
	socketClient client;
public:
	Machine(); // ������
	~Machine(); // �Ҹ���
	void Coin_Input(); // ���� �Է�
	void Sell(int*); // ����� �Ǹ�
	void Change(int*); // �Ž�����
	void Menu(); // �޴�
	void Admin_Menu(); // �����ڸ޴�
	void Sales(); // ����
	void Modify_Name(); // ����� �̸� ����
	void Modify_Cost(); // ����� ���� ����
	void Modify_Stock(); // ����� ��� Ȯ��, ����
	void Collect_Money(); // ����
	void Sold_Out(); // ����� ���� ��¥ Ȯ��
	void Save(); // ����
	void Load(); // �ҷ�����
};
Machine::Machine() {
	Node* NewNode;
	// ó�� ��� ������ ���� 3��
	for (int k = 0; k < 5; k++) {
		Stock[k] = NULL;
		for (int i = 0; i < 3; i++) {
			NewNode = Create();
			Insert(&Stock[k], NewNode);
		}
	}
	// ������ �̸����� ����
	strcpy(Name[0], "��");
	strcpy(Name[1], "Ŀ��");
	strcpy(Name[2], "�̿�����");
	strcpy(Name[3], "���Ŀ��");
	strcpy(Name[4], "ź������");
	// ������ ���ݵ��� ����
	Cost[0] = 450;
	Cost[1] = 500;
	Cost[2] = 550;
	Cost[3] = 700;
	Cost[4] = 750;
	// ���Ǳ��� �Ž����� �ʱ�ȭ
	Coin_10 = 5;
	Coin_50 = 5;
	Coin_100 = 5;
	Coin_500 = 5;
	Coin_1000 = 5;
	count_1000 = 0;
	// �Էµ� ���� �� �ʱ�ȭ
	Coin_Sum = 0;
	// �Ϻ�, ���� ���� �ʱ�ȭ
	for (int i = 0; i < 13; i++) {
		Sale_month[i] = 0;
		for (int j = 0; j < 32; j++) {
			Sale_day[i][j] = 0;
			for (int k = 0; k < 5; k++) {
				Sale_drink[k][i][j] = 0;
			}
		}
	}
	// ���� ��¥ ���ϱ�
	time(&t);
	localtime_s(&A, &t);
	month = A.tm_mon + 1;
	day = A.tm_mday;
	Load(); // �ҷ������Լ� ȣ��
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
	if (client.data.eco == 1) { // �̹� ������ ���������� �־ eco�� �޾Ҵٸ�
		for (int i = 0; i < 5; i++) { // �������� ������ �̸����� �о�� �������ش�.
			strcpy(Name[i], client.data.Name[i]);
		}
	}
}
Machine::~Machine() {
	Save();
}
void Machine::Save() {
	ofstream out("data.txt");
	// ����� �̸� ����
	for (int i = 0; i < 5; i++)
		out << Name[i] << endl;
	// ����� ���� ����
	for (int i = 0; i < 5; i++)
		out << Cost[i] << endl;
	// ����� ��� ����
	for (int i = 0; i < 5; i++) {
		out << Count(Stock[i]) << endl;
	}
	// ���Ǳ��� �Ž��� �� ����
	out << Coin_10 << endl;
	out << Coin_50 << endl;
	out << Coin_100 << endl;
	out << Coin_500 << endl;
	out << Coin_1000 << endl;
	// ���� ���� ����
	for (int i = 1; i <= 12; i++)
		out << Sale_month[i] << " ";
	// �Ϻ� ���� ����
	for (int i = 1; i <= 12; i++)
		for (int j = 1; j <= 31; j++)
			out << Sale_day[i][j] << " ";
	// ������� �Ϻ� ���� ����
	for (int k = 0; k < 5; k++)
		for (int i = 1; i <= 12; i++)
			for (int j = 1; j <= 31; j++)
				out << Sale_drink[k][i][j] << " ";
}
void Machine::Load() {
	ifstream in("data.txt");
	if (in.eof() || !in.is_open())return; // ������ ���ų� ������ ������ ������
	// ����� �̸� �ҷ�����
	for (int i = 0; i < 5; i++)
		in >> Name[i];
	// ����� ���� �ҷ�����
	for (int i = 0; i < 5; i++)
		in >> Cost[i];
	// ����� ��� �ҷ�����
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
	// ���Ǳ��� �Ž��� �� �ҷ�����
	in >> Coin_10;
	in >> Coin_50;
	in >> Coin_100;
	in >> Coin_500;
	in >> Coin_1000;
	// ���� ���� �ҷ�����
	for (int i = 1; i <= 12; i++)
		in >> Sale_month[i];
	// �Ϻ� ���� �ҷ�����
	for (int i = 1; i <= 12; i++)
		for (int j = 1; j <= 31; j++)
			in >> Sale_day[i][j];
	// ������� �Ϻ� ���� �ҷ�����
	for (int k = 0; k < 5; k++)
		for (int i = 1; i <= 12; i++)
			for (int j = 1; j <= 31; j++)
				in >> Sale_drink[k][i][j];
	// �Էµ� ���� �� �ʱ�ȭ
	Coin_Sum = 0;
	count_1000 = 0;
}
void Machine::Coin_Input() {
	int* Coin_Check;
	char Input_Temp[10];
	cout << "�Է��Ͻ� ȭ���� ������ �Է��ϼ���. (10��, 50��, 100��, 500��, 1000��) ";
	Coin_Check = new int; // �ԷµǴ� ���������� �����Ҵ� �Ѵ�.
	try {
		cin >> Input_Temp;
		*Coin_Check = atoi(Input_Temp);
		switch (*Coin_Check) {
		case 1000:
			count_1000++;
			if (count_1000 > 3) {
				cout << "������ ���Ѽ��� 3000������ �Դϴ�." << endl;
			}
			else if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_1000 = Coin_1000 + 1; // ���Ǳ��� 1000�� ���� ������ ���Ͽ� ����
				Coin_Sum += *Coin_Check;
			}
			else { // �Է��� ����� ���� �ʾ��� ���
				throw 'e';
			}
			break;
		case 500:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_500 = Coin_500 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // �Է��� ����� ���� �ʾ��� ���
				throw 'e';
			}
			break;
		case 100:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_100 = Coin_100 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // �Է��� ����� ���� �ʾ��� ���
				throw 'e';
			}
			break;
		case 50:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_50 = Coin_50 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // �Է��� ����� ���� �ʾ��� ���
				throw 'e';
			}
			break;
		case 10:
			if (*Coin_Check + Coin_Sum <= 5000) {
				Coin_10 = Coin_10 + 1;
				Coin_Sum += *Coin_Check;
			}
			else { // �Է��� ����� ���� �ʾ��� ���
				throw 'e';
			}
			break;
		default:
			throw 'e';
		}
	}
	catch (char e) {
		cout << "�Է¿� ������ �ֽ��ϴ�." << endl;
	}
	free(Coin_Check); // ���� �Է��� ������ �����Ҵ��� �����Ѵ�.
}
void Machine::Sell(int* money) {
	// ����� �޴� ���
	cout << "------------------------------" << endl;
	for (int i = 0; i < 5; i++) {
		cout << i << ". " << setw(10) << Name[i] << " (" << Cost[i] << ")";
		if (*money >= Cost[i] && Count(Stock[i]) != 0) cout << setw(20) << "����� �� ����." << endl;
		else {
			if (Count(Stock[i]) == 0) cout << setw(20) << "����" << endl;
			else cout << setw(20) << "�ݾ��� ���ڶ�" << endl;
		}
	}
	///////////////////////////////////////////////////////////////
	cout << " �޴��� �Է��ϼ���. ";
	int Choice;
	cin >> Choice;
	if (*money >= Cost[Choice] && Count(Stock[Choice]) != 0) {
		cout << "����� " << Name[Choice] << "��(��) ������ϴ�." << endl;
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
		if (Count(Stock[Choice]) == 0) cout << "�����Ǿ����ϴ�." << endl;
		else cout << "�ݾ��� ���ڶ��ϴ�." << endl;
	}
}
void Machine::Change(int* money) {
	int c1000, c500, c100, c50, c10;
	c1000 = c500 = c100 = c50 = c10 = 0; // �Ž��� ���� ī��Ʈ
	while (*money != 0) { // ���Ե� ���� 0�� �ɶ�����
		if (*money >= 1000 && Coin_1000 > 0) { // 1000�� �̻��� �ְ� 1000�� ¥���� �Ž��� �� �� �������
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
		else { // �Ž������� �������?
			cout << "�Ž������� �����ϴ�." << endl;
		}
	}
	if (c1000 != 0)
		cout << "1000�� ���� : " << c1000 << "��" << endl;
	if (c500 != 0)
		cout << "500�� ���� : " << c500 << "��" << endl;
	if (c100 != 0)
		cout << "100�� ���� : " << c100 << "��" << endl;
	if (c50 != 0)
		cout << "50�� ���� : " << c50 << "��" << endl;
	if (c10 != 0)
		cout << "10�� ���� : " << c10 << "��" << endl;
	cout << "��ȯ �Ǿ����ϴ�." << endl;
	count_1000 = 0; // õ�� ¥�� ī��Ʈ�� �ʱ�ȭ
}
void Machine::Menu() {
	int Choice;
	char Input_Temp[10];
	for (;;) {
		cout << " " << month << "/" << day << endl;
		cout << "-------------Menu------------" << endl;
		cout << " 1. ���� ���� " << endl;
		cout << " 2. ����� ����" << endl;
		cout << " 3. �Ž����� ��ȯ" << endl;
		cout << " 4. ������ �޴�" << endl;
		cout << " 0. ����" << endl;
		cout << "-----------------------------" << endl;
		cout << " ���� ���Ե� �ݾ� : " << Coin_Sum << endl;
		cout << " �޴��� �Է����ּ���. ";
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
			cout << "�Է¿� ������ �ֽ��ϴ�." << endl;
		}
		cout << "�ƹ� Ű�� �Է����ּ���." << endl;
		char c = getch();
		system("CLS");
	}
}
void Machine::Admin_Menu() {
	char password[5] = { "1234" };
	char Input_Temp[10];
	char input_pass[5] = { 0 };
	int Choice;
	cout << "��й�ȣ�� �Է����ּ��� : ";
	for (int i = 0; i < 4; i++) {
		input_pass[i] = getch();
		cout << "*";
	}
	cout << endl;
	if (strcmp(password, input_pass) != 0) {
		cout << "��й�ȣ�� Ʋ�Ƚ��ϴ�." << endl;
		return;
	}
	for (;;) {
		cout << "-------------Menu------------" << endl;
		cout << " 1. ���� ��� " << endl;
		cout << " 2. ����� �̸� ����" << endl;
		cout << " 3. ����� ���� ����" << endl;
		cout << " 4. ����� ��� �߰� " << endl;
		cout << " 5. ����� ���� ��¥ " << endl;
		cout << " 6. ���� " << endl;
		cout << " 0. �ڷΰ���" << endl;
		cout << "-----------------------------" << endl;
		cout << " �޴��� �Է��ϼ���. ";
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
			cout << "�Է¿� ������ �ֽ��ϴ�." << endl;
		}
		cout << "�ƹ� Ű�� �Է����ּ���." << endl;
		char c = getch();
		system("CLS");
	}
}
void Machine::Modify_Name() {
	cout << "���� ����� �̸� : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Cost[i] << "��" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "� ������� �ٲٽðڽ��ϱ�? ";
	char change[30];
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "�ٲٽ� �̸���? ";
			cin >> change;
			strcpy(Name[i], change);
			cout << "������ �Ϸ�Ǿ����ϴ�." << endl;
			break;
		}
	}
}
void Machine::Modify_Cost() {
	cout << "���� ����� ���� : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Cost[i] << "��" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "� ������� �ٲٽðڽ��ϱ�?";
	char change[30];
	int change_cost;
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "�󸶷� �ٲٽðڽ��ϱ�?";
			cin >> change_cost;
			Cost[i] = change_cost;
			cout << "������ �Ϸ�Ǿ����ϴ�." << endl;
			break;
		}
	}
}
void Machine::Collect_Money() {
	int collect, sum;
	char Input_Temp[10];
	sum = 0;
	cout << "õ��¥�� ���� " << Coin_1000 << "�� �ֽ��ϴ�." << endl;
	cout << "������ �����Ͻðڽ��ϱ�? (�ּ��� 5���� �����־�� �մϴ�.) " << endl;
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
		cout << "�����¥�� " << Coin_500 << "�� �ֽ��ϴ�." << endl;
		cout << "��� �����Ͻðڽ��ϱ�? (�ּ��� 5���� �����־�� �մϴ�.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_500 - 5 >= collect) {
			sum += collect * 500;
			Coin_500 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "���¥�� " << Coin_100 << "�� �ֽ��ϴ�." << endl;
		cout << "��� �����Ͻðڽ��ϱ�? (�ּ��� 5���� �����־�� �մϴ�.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_100 - 5 >= collect) {
			sum += collect * 100;
			Coin_100 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "���ʿ�¥�� " << Coin_50 << "�� �ֽ��ϴ�." << endl;
		cout << "��� �����Ͻðڽ��ϱ�? (�ּ��� 5���� �����־�� �մϴ�.) " << endl;
		cin >> Input_Temp;
		collect = atoi(Input_Temp);
		if (Coin_50 - 5 >= collect) {
			sum += collect * 50;
			Coin_50 -= collect;
		}
		else {
			throw 'e';
		}
		cout << "�ʿ�¥�� " << Coin_10 << "�� �ֽ��ϴ�." << endl;
		cout << "��� �����Ͻðڽ��ϱ�? (�ּ��� 5���� �����־�� �մϴ�.) " << endl;
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
		cout << "�Է� �����Դϴ�." << endl;
	}
	cout << "�� ������ �ݾ��� : " << sum << "�Դϴ�." << endl;
}
void Machine::Sold_Out() {
	int check;
	cout << "����� ���� ��¥ : " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << " ���� ��¥ : " << endl;
		for (int j = 1; j <= 12; j++) {
			for (int k = 1; k <= 12; k++) {
				if (Sold_out[i][j][k] == 1) {
					cout << setw(8) << j << "�� " << k << "��" << endl;
				}
			}
		}
		cout << "------------------------------" << endl;
	}
}
void Machine::Modify_Stock() {
	cout << "���� ����� ��� " << endl;
	for (int i = 0; i < 5; i++) {
		cout << setw(8) << Name[i] << setw(10) << Count(Stock[i]) << "��" << endl;
	}
	cout << "-----------------------------" << endl;
	cout << "� ������� ��� �߰��Ͻðڽ��ϱ�? ";
	char change[30];
	int add;
	cin >> change;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Name[i], change) == 0) {
			cout << "��� �� �߰��Ͻðڽ��ϱ�?";
			cin >> add;
			if (add > 0) {
				Node* NewNode;
				for (int j = 0; j < add; j++) {
					NewNode = Create();
					Insert(&Stock[i], NewNode);
				}
				cout << "�߰��� �Ϸ�Ǿ����ϴ�." << endl;
			}
			else {
				cout << "�Է� �����Դϴ�." << endl;
			}
			break;
		}
	}
}
void Machine::Sales() { // ���� ���
	int i, j;
	// ���� ����
	for (i = 1; i <= 12; i++) {
		cout << i << "�� ���� : " << Sale_month[i] << endl;
	}
	int check;
	// �Ϻ� ����
	for (i = 1; i <= 12; i++) {
		check = 0;
		for (j = 1; j <= 31; j++) {
			if (Sale_day[i][j] != 0) {
				cout << i << "�� " << j << "�� ���� : " << Sale_day[i][j] << endl;
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