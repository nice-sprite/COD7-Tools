#include "HookManager.h"
#include "DxHook.h"


HookManager DxHook::hookManager;
std::unordered_map<int, HOOKINFO> DxHook::hooks;

constexpr DWORD ENDSCENE_IDX = 42;
constexpr DWORD BEGINSCENE_IDX = 41;
constexpr DWORD DIP_IDX = 82;
constexpr DWORD PRESENT_IDX = 17;

void* DxHook::deviceVtbl[119]{};
IDirect3DDevice9* DxHook::pDevice{ nullptr };


EndScene_t DxHook::oEndScene{ nullptr };
BeginScene_t DxHook::oBeginScene{ nullptr };
Present_t DxHook::oPresent{ nullptr };
Reset_t DxHook::oReset{ nullptr };
DrawIndexedPrimitive_t DxHook::oDIP{ nullptr };

HOOKINFO DxHook::ESHInfo = {};

HANDLE DxHook::hDeviceCapturedEvent = 0;


void DxHook::ResetCaptureEvent()
{
	if (hDeviceCapturedEvent)
	{
		CloseHandle(hDeviceCapturedEvent);
		hDeviceCapturedEvent = {};
	}
	hDeviceCapturedEvent = CreateEvent(0, TRUE, false, 0);
}


bool DxHook::WaitForDevice(int milliseconds)
{
	// wait for object to signal that the hook code has ran and the device was captured (implement using WaitForSingleObject())
	switch (WaitForSingleObject(hDeviceCapturedEvent, milliseconds))
	{
	case WAIT_OBJECT_0:
		return true;
		break;
	case WAIT_TIMEOUT:
	case WAIT_ABANDONED:
	case WAIT_FAILED:
	default:
		break;
	}
	return false;
}


bool DxHook::CopyVMT()
{
	if (DxHook::deviceVtbl[0] != nullptr) return true;
	using __Direct3DCreate9 = IDirect3D9*(__stdcall*)(UINT);
	__Direct3DCreate9 d3d9create = (__Direct3DCreate9)(GetProcAddress(GetModuleHandle("d3d9"), "Direct3DCreate9"));
	IDirect3D9* pD3D = d3d9create(D3D_SDK_VERSION);
	if (!pD3D)
		return false;

	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetForegroundWindow();
	d3dpp.Windowed = ((GetWindowLong(d3dpp.hDeviceWindow, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;

	IDirect3DDevice9* pDummyDevice = nullptr;
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice)))
	{
		pD3D->Release();
		return false;
	}

	DWORD oldProt;
	VirtualProtect(*(void***)pDummyDevice, sizeof(deviceVtbl), PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy(deviceVtbl, *(void***)pDummyDevice, sizeof(deviceVtbl));
	VirtualProtect(*(void***)pDummyDevice, sizeof(deviceVtbl), oldProt, &oldProt);

	SafeRelease(&pDummyDevice);
	SafeRelease(&pD3D);

	return true;
}


bool DxHook::CacheDeviceViaEndScene()
{
	CopyVMT();
	ResetCaptureEvent();

	ESHInfo = hookManager.Trampoline((BYTE*)deviceVtbl[ENDSCENE_IDX], (BYTE*)nEndScene);
	oEndScene = (EndScene_t)ESHInfo.gateway;
	hooks.insert({ ENDSCENE_IDX, ESHInfo });

	return WaitForDevice() && pDevice != nullptr;
}


bool DxHook::CacheDeviceViaBeginScene()
{
	CopyVMT();
	ResetCaptureEvent();

	auto info = hookManager.Trampoline((BYTE*)deviceVtbl[BEGINSCENE_IDX], (BYTE*)nBeginScene);
	oBeginScene = (BeginScene_t)info.gateway;
	hooks.insert({ BEGINSCENE_IDX, info });
	return WaitForDevice() && pDevice != nullptr;
}


bool DxHook::CacheDeviceViaDIP()
{
	CopyVMT();
	ResetCaptureEvent();

	auto info = hookManager.Trampoline((BYTE*)deviceVtbl[DIP_IDX], (BYTE*)nDrawIndexedPrimitive);
	oDIP = (DrawIndexedPrimitive_t)info.gateway;
	hooks.insert({ DIP_IDX, info });

	return WaitForDevice() && pDevice != nullptr;
}


bool DxHook::CacheDeviceViaPresent()
{
	CopyVMT();
	ResetCaptureEvent();

	auto info = hookManager.Trampoline((BYTE*)deviceVtbl[PRESENT_IDX], (BYTE*)nPresent);
	oPresent = (Present_t)info.gateway;
	hooks.insert({ PRESENT_IDX, info });

	return WaitForDevice() && pDevice != nullptr;
}


bool DxHook::CacheDeviceViaHailMarry()
{
	CopyVMT();

	return false;
}







void DxHook::CleanUpAndShutdown()
{
	DWORD oldProt;
	if (oEndScene != nullptr)
	{
		VirtualProtect((*(void***)pDevice)[42], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProt);
		(*(void***)pDevice)[42] = (void*)oEndScene;
		VirtualProtect((*(void***)pDevice)[42], sizeof(void*), oldProt, &oldProt);
	}
}

HRESULT WINAPI nEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	DxHook::pDevice = pDevice;
	auto ret = DxHook::oEndScene(pDevice);
	DxHook::hookManager.UninstallDetour(DxHook::ESHInfo); // patches back the original bytes of the function
	SetEvent(DxHook::hDeviceCapturedEvent);
	return ret;
}


HRESULT __stdcall nBeginScene(LPDIRECT3DDEVICE9 pDevice)
{
	DxHook::pDevice = pDevice;
	auto ret = DxHook::oBeginScene(pDevice);
	DxHook::hookManager.UninstallDetour(DxHook::ESHInfo); // patches back the original bytes of the function
	SetEvent(DxHook::hDeviceCapturedEvent);

	return E_NOTIMPL;
}

HRESULT __stdcall nPresent(LPDIRECT3DDEVICE9 pDevice)
{
	DxHook::pDevice = pDevice;
	auto ret = DxHook::oPresent(pDevice);
	return E_NOTIMPL;
}

HRESULT __stdcall nReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pp)
{
	DxHook::pDevice = pDevice;
	auto ret = DxHook::oReset(pDevice, pp);
	return E_NOTIMPL;
}

HRESULT __stdcall nDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	DxHook::pDevice = pDevice;
	auto ret = DxHook::oDIP(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	return E_NOTIMPL;
}
