#pragma once
#include <d3d9.h>


#pragma comment(lib, "d3d9.lib")

#include "HookManager.h"
#include <unordered_map>
#include <functional>

typedef HRESULT(WINAPI* EndScene_t)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(WINAPI* BeginScene_t)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(WINAPI* Present_t)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(WINAPI* Reset_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT(WINAPI* DrawIndexedPrimitive_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE,
	INT              BaseVertexIndex,
	UINT             MinVertexIndex,
	UINT             NumVertices,
	UINT             startIndex,
	UINT             primCount);

HRESULT WINAPI nEndScene(LPDIRECT3DDEVICE9 pDevice);
HRESULT WINAPI nBeginScene(LPDIRECT3DDEVICE9 pDevice);
HRESULT WINAPI nPresent(LPDIRECT3DDEVICE9 pDevice);
HRESULT WINAPI nReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS*);
HRESULT WINAPI nDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType,
	INT              BaseVertexIndex,
	UINT             MinVertexIndex,
	UINT             NumVertices,
	UINT             startIndex,
	UINT             primCount);



template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

enum D3DVTABLE_INDEX {
	iQueryInterface,
	iAddRef,
	iRelease,
	iTestCooperativeLevel,
	iGetAvailableTextureMem,
	iEvictManagedResources,
	iGetDirect3D,
	iGetDeviceCaps,
	iGetDisplayMode,
	iGetCreationParameters,
	iSetCursorProperties,
	iSetCursorPosition,
	iShowCursor,
	iCreateAdditionalSwapChain,
	iGetSwapChain,
	iGetNumberOfSwapChains,
	iReset,
	iPresent,
	iGetBackBuffer,
	iGetRasterStatus,
	iSetDialogBoxMode,
	iSetGammaRamp,
	iGetGammaRamp,
	iCreateTexture,
	iCreateVolumeTexture,
	iCreateCubeTexture,
	iCreateVertexBuffer,
	iCreateIndexBuffer,
	iCreateRenderTarget,
	iCreateDepthStencilSurface,
	iUpdateSurface,
	iUpdateTexture,
	iGetRenderTargetData,
	iGetFrontBufferData,
	iStretchRect,
	iColorFill,
	iCreateOffscreenPlainSurface,
	iSetRenderTarget,
	iGetRenderTarget,
	iSetDepthStencilSurface,
	iGetDepthStencilSurface,
	iBeginScene,
	iEndScene,
	iClear,
	iSetTransform,
	iGetTransform,
	iMultiplyTransform,
	iSetViewport,
	iGetViewport,
	iSetMaterial,
	iGetMaterial,
	iSetLight,
	iGetLight,
	iLightEnable,
	iGetLightEnable,
	iSetClipPlane,
	iGetClipPlane,
	iSetRenderState,
	iGetRenderState,
	iCreateStateBlock,
	iBeginStateBlock,
	iEndStateBlock,
	iSetClipStatus,
	iGetClipStatus,
	iGetTexture,
	iSetTexture,
	iGetTextureStageState,
	iSetTextureStageState,
	iGetSamplerState,
	iSetSamplerState,
	iValidateDevice,
	iSetPaletteEntries,
	iGetPaletteEntries,
	iSetCurrentTexturePalette,
	iGetCurrentTexturePalette,
	iSetScissorRect,
	iGetScissorRect,
	iSetSoftwareVertexProcessing,
	iGetSoftwareVertexProcessing,
	iSetNPatchMode,
	iGetNPatchMode,
	iDrawPrimitive,
	iDrawIndexedPrimitive,
	iDrawPrimitiveUP,
	iDrawIndexedPrimitiveUP,
	iProcessVertices,
	iCreateVertexDeclaration,
	iSetVertexDeclaration,
	iGetVertexDeclaration,
	iSetFVF,
	iGetFVF,
	iCreateVertexShader,
	iSetVertexShader,
	iGetVertexShader,
	iSetVertexShaderConstantF,
	iGetVertexShaderConstantF,
	iSetVertexShaderConstantI,
	iGetVertexShaderConstantI,
	iSetVertexShaderConstantB,
	iGetVertexShaderConstantB,
	iSetStreamSource,
	iGetStreamSource,
	iSetStreamSourceFreq,
	iGetStreamSourceFreq,
	iSetIndices,
	iGetIndices,
	iCreatePixelShader,
	iSetPixelShader,
	iGetPixelShader,
	iSetPixelShaderConstantF,
	iGetPixelShaderConstantF,
	iSetPixelShaderConstantI,
	iGetPixelShaderConstantI,
	iSetPixelShaderConstantB,
	iGetPixelShaderConstantB,
	iDrawRectPatch,
	iDrawTriPatch,
	iDeletePatch,
	iCreateQuery,
	iSetConvolutionMonoKernel,
	iComposeRects,
	iPresentEx,
	iGetGPUThreadPriority,
	iSetGPUThreadPriority,
	iWaitForVBlank,
	iCheckResourceResidency,
	iSetMaximumFrameLatency,
	iGetMaximumFrameLatency,
	iCheckDeviceState,
	iCreateRenderTargetEx,
	iCreateOffscreenPlainSurfaceEx,
	iCreateDepthStencilSurfaceEx,
	iResetEx,
	iGetDisplayModeEx
};

class DxHook
{

public:
	static inline void ResetCaptureEvent();
	static inline bool WaitForDevice(int milliseconds = 200);

	/* Copies the vtable of the dummy device */
	static bool CopyVMT();

	/* installs midfunction hook in EndScene to
	Cache the IDirect3DDevice9 used by the game

	returns true if the device was able to be cached
	*/
	static bool CacheDeviceViaEndScene();

	/* installs midfunction hook in BeginScene to
	Cache the IDirect3DDevice9 used by the game

	returns true if the device was able to be cached
	*/
	static bool CacheDeviceViaBeginScene();

	/* installs midfunction hook in DrawIndexedPrimative to
	Cache the IDirect3DDevice9 used by the game

	returns true if the device was able to be cached
	*/
	static bool CacheDeviceViaDIP();

	/* installs midfunction hook in Present to
	Cache the IDirect3DDevice9 used by the game

	returns true if the device was able to be cached
	*/
	static bool CacheDeviceViaPresent();

	/* Tries every available method for caching the IDirect3DDevice9

	returns true if the device was able to be cached
	*/
	static bool CacheDeviceViaHailMarry();

	// executes the provided function with PAGE_EXECUTE_READWRITE privs
	template<typename T>
	static T WithMemPrivledges(std::function<T()> func, void* loc, size_t size = sizeof(void*))
	{
		DWORD protect;
		VirtualProtect(loc, size, PAGE_EXECUTE_READWRITE, &protect);
		auto ret = func();
		VirtualProtect(loc, size, protect, &protect);
		return ret;
	}

public:
	static HookManager hookManager;

	static void* deviceVtbl[119];
	static IDirect3DDevice9* pDevice;


	static Reset_t oReset;
	static Present_t oPresent;
	static EndScene_t oEndScene;
	static BeginScene_t oBeginScene;
	static DrawIndexedPrimitive_t oDIP;

	static HOOKINFO ESHInfo;
	static std::unordered_map<int, HOOKINFO> hooks;

	static HANDLE hDeviceCapturedEvent;

	static DWORD beginSceneRet;

public:

	template<typename T>
	static T Hook(D3DVTABLE_INDEX idx, T dst)
	{
		if (!CopyVMT() || !dst) return nullptr;
		return (T)hookManager.Trampoline((BYTE*)deviceVtbl[idx], (BYTE*)dst).gateway;
	}

	template<typename T>
	static T VirtualHook(D3DVTABLE_INDEX idx, T* dst)
	{
		if (!pDevice)
		{
			CacheDeviceViaEndScene();
		}
		if (!CopyVMT() || !dst || !pDevice) return nullptr;
		DWORD prot;
		return DxHook::WithMemPrivledges<T>([=]()->T {
			T ret = (T)(*(void***)pDevice)[idx];
			(*(void***)pDevice)[idx] = dst;
			return ret;
			}, (*(void***)pDevice)[idx]);
	}
	static void CleanUpAndShutdown();

	static std::string Debug();
	
};

