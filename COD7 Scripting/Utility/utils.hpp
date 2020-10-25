#pragma once
#include <Windows.h>
#include <string>
#include <fcntl.h>
#include <io.h>
#include <iostream>

void DebugPrint(std::string toPrint);

struct lpProcess
{
	DWORD pID = GetProcessId(GetCurrentProcess());
	HWND hwnd = NULL;
};

BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam);

HWND GetProcessHwnd();
