#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>
#include "../HearthMirror/Mirror.hpp"

using namespace hearthmirror;

DWORD GetProcId(WCHAR* ProcName);

int main(int argc, char *argv[]) {
	printf("HearthMirror alpha 0.5\n");

	// get pid of Hearthstone
	DWORD pid = GetProcId(L"Hearthstone.exe");
	printf("Hearthstone PID: %u\n", pid);

	Mirror* mirror = new Mirror(pid);
}

DWORD GetProcId(WCHAR* ProcName) {
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32))
	{
		do {
			if (wcscmp(pe32.szExeFile, ProcName) == 0)
				break;
		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);

	return pe32.th32ProcessID;
}