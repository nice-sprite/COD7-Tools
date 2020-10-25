#include "utils.hpp"

void DebugPrint(std::string toPrint)
{
	static bool bAllocated = AllocConsole();
	if (bAllocated)
	{
		SetConsoleTitle("debug log");
		static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		int consoleHandleW = _open_osfhandle((intptr_t)hOut, _O_TEXT);
		FILE* fptr = _fdopen(consoleHandleW, "w");
		*stdout = *fptr;
		setvbuf(stdout, NULL, _IONBF, 0);

		bAllocated = false; // on second call, we shouldnt re-run all the code 
	}

	std::cout << toPrint;

}

BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	lpProcess* lp = (lpProcess*)lParam;
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
	lpProcess lp;
	EnumWindows(_EnumWindowsProc, (LPARAM)&lp);
	return lp.hwnd;
}
