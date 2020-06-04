#pragma once
#include <Windows.h>

struct lparam
{
	DWORD pID = GetProcessId(GetCurrentProcess());
	HWND hwnd = NULL;
};

BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	lparam* lp = (lparam*)lParam;
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == (DWORD)lp->pID)
	{
		lp->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND GetProcessHwnd()
{
	lparam lp;
	EnumWindows(_EnumWindowsProc, (LPARAM)&lp);
	return lp.hwnd;
}
