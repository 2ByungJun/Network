#include <Windows.h>
#include <stdio.h>

DWORD WINAPI MyThread(LPVOID arg) {
	while (1) { printf("�ݺ� ������2!!!\n"); };
	return 0;
}

int main() {
	// CPU ������ �˾Ƴ���.
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// CPU ������ŭ �����带 �����Ѵ�.
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++) {
		HANDLE hTread = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
		if (hTread = NULL) return 1;
		// �ְ� �켱 ������ �����Ѵ�.
		SetThreadPriority(hTread, THREAD_PRIORITY_TIME_CRITICAL);
		CloseHandle(hTread);
	}

	Sleep(1000);
	while (1) { printf("�� ������ ����!\n"); break; }

	return 0;
}