#include "HookManager.h"
#include <capstone/capstone.h>
int HookManager::CalcHookSize(BYTE* hk)
{
	csh handle;
	cs_insn* insn;
	size_t count;

	if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
		return -1;

	count = cs_disasm(handle, hk, 20, 0x1000, 0, &insn);

	int hookSize = 0;
	for (int i = 0; i < count && hookSize < 5; i++)
	{
		hookSize += insn[i].size;
	}

	// clean up capstone
	cs_free(insn, count);
	cs_close(&handle);
	return hookSize;
}

HOOKINFO HookManager::InstallDetour(BYTE* src, BYTE* dst, std::string funcName, size_t size)
{
	if (size < 5)
	{
		size = CalcHookSize(src);
	}

	HOOKINFO hookInfo;
	hookInfo.src = src;
	hookInfo.dst = dst;
	hookInfo.originalBytesLength = size;
	hookInfo.debugFunctionSymbol = funcName;

	DWORD oldProtect;
	DWORD newProtect;
	DWORD relativeAddress;
	VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy_s(hookInfo.originalBytes, size, src, size);
	relativeAddress = (DWORD)(dst - src) - 5;
	*src = 0xE9;
	*((DWORD*)(src + 0x1)) = relativeAddress;
	for (DWORD x = 0x5; x < size; x++)
		*(src + x) = 0x90;

	hookInfo.isHookInstalled = true;

	VirtualProtect(src, size, oldProtect, &newProtect);

	this->hookInfo.push_back(hookInfo);
	return hookInfo;
}

HOOKINFO HookManager::InstallDetour(HOOKINFO hookInfo)
{
	return InstallDetour(hookInfo.src,
		hookInfo.dst,
		hookInfo.debugFunctionSymbol,
		hookInfo.originalBytesLength);
}

HOOKINFO HookManager::Trampoline(BYTE* src, BYTE* dst, std::string funcName, size_t size)
{
	if (size < 5)
	{
		size = CalcHookSize(src);
	}

	BYTE* gateway = (BYTE*)VirtualAlloc(NULL, size + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy_s(gateway, size, src, size);

	*(gateway + size) = 0xE9;
	*(DWORD*)(gateway + size + 0x1) = (src - gateway) - 5;
	auto info = InstallDetour(src, dst, funcName, size);
	info.gateway = gateway;
	return info;
}

HOOKINFO HookManager::UninstallDetour(BYTE* const src, BYTE* originalBytes, size_t size)
{
	HOOKINFO hookInfo;
	hookInfo.src = src;
	hookInfo.originalBytesLength = size;
	hookInfo.dst = 0;
	hookInfo.isHookInstalled = false;
	memcpy(hookInfo.originalBytes, originalBytes, size);
	DWORD oldProtect;
	VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(src, originalBytes, size);
	VirtualProtect(src, size, oldProtect, &oldProtect);
	return hookInfo;
}

HOOKINFO HookManager::UninstallDetour(HOOKINFO hookInfo)
{
	OutputDebugString(hookInfo.debugFunctionSymbol.c_str());
	return UninstallDetour(hookInfo.src, hookInfo.originalBytes, hookInfo.originalBytesLength);
}

HookInfoList HookManager::InstallMultipleDetours(HookInfoList& hookList)
{
	HookInfoList list;
	for (auto& entry : hookList)
	{
		if (!entry.isHookInstalled)
		{
			list.push_back(InstallDetour(entry));
		}
	}
	this->hookInfo.insert(this->hookInfo.end(), hookInfo.begin(), hookInfo.end());
	return list;
}

HookInfoList HookManager::UninstallMultipleDetours(HookInfoList& hookList)
{
	HookInfoList list;
	for (auto& entry : hookList)
	{
		if (entry.isHookInstalled)
		{
			list.push_back(UninstallDetour(entry));
		}
	}
	this->hookInfo.insert(this->hookInfo.end(), hookInfo.begin(), hookInfo.end());
	return list;
}

bool HookManager::UninstallAllDetours()
{
	bool allUninstalled = false;
	auto List = UninstallMultipleDetours(hookInfo);
	for (auto& entry : List)
	{
		allUninstalled |= entry.isHookInstalled;
	}
	return allUninstalled;
}

HookManager::~HookManager()
{
	//UninstallAllDetours();
}

