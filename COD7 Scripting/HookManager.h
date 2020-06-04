#pragma once
#include <Windows.h>
#include <vector>
#include <string>

struct HOOKINFO
{
	BYTE* src;
	BYTE* dst;
	BYTE* gateway;
	BYTE originalBytes[32]; 
	size_t originalBytesLength;
	bool isHookInstalled;
	std::string debugFunctionSymbol; // used for easily determining what function was hooked while debugging
};

typedef std::vector<HOOKINFO> HookInfoList;


class HookManager
{
	HookInfoList hookInfo;
	int CalcHookSize(BYTE* hk);
public:

	HOOKINFO InstallDetour(BYTE* src, BYTE* detourFunction, std::string funcName = "", size_t size = 0);
	HOOKINFO InstallDetour(HOOKINFO hookInfo);
	HOOKINFO Trampoline(BYTE* source, BYTE* dst, std::string funcName = "", size_t size = 0);
	

	HOOKINFO UninstallDetour(BYTE* src, BYTE* originalBytes, size_t size);
	HOOKINFO UninstallDetour(HOOKINFO hookInfo);

	

	HookInfoList InstallMultipleDetours(HookInfoList& hookList);
	HookInfoList UninstallMultipleDetours(HookInfoList& hookList);
	bool UninstallAllDetours();

	~HookManager();

};

