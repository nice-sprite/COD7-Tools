#include "common.hpp"
#include "./Utility/utils.hpp"
#include "./Engine/Engine.h"
#include "./Engine/callbacks.h"
#include "./Graphics/ImGuiConsole.h"


#include <fmt/core.h>

// prototypes
void MainThread(HMODULE DLL);

HRESULT __stdcall _EndScene(IDirect3DDevice9* pDevice);
HRESULT __stdcall _Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall ImguiWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);




// global vars
HWND g_HWND = NULL;
DWORD g_MainThreadID = 0;
HANDLE g_ThreadHandle = 0;
ImGuiContext* g_imguiCtx = nullptr;
CircularBuffer<float> frames(512);

//

// original hook vars
WNDPROC _WndProc;
EndScene_t endscene;
Reset_t reset;
//


BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		g_ThreadHandle = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hinstDLL, NULL, &g_MainThreadID);
		if (!g_ThreadHandle) return FALSE;
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}

HookManager g_Hookman;
void MainThread(HMODULE DLL)
{
	
	// setup
	g_HWND = GetProcessHwnd();
	_WndProc = (WNDPROC)GetWindowLongPtr(g_HWND, GWL_WNDPROC);
	SetWindowLongPtr(g_HWND, GWL_WNDPROC, (LONG_PTR)ImguiWndProc);
	DxHook::CacheDeviceViaEndScene();
	endscene = DxHook::Hook<EndScene_t>(D3DVTABLE_INDEX::iEndScene, _EndScene);
	reset = DxHook::VirtualHook<Reset_t>(D3DVTABLE_INDEX::iReset, (Reset_t*)_Reset);

	// install hack hooks
	Engine::CenterCursorPos = (CenterCursorPos_t)g_Hookman.Trampoline(
		(BYTE*)Engine::CenterCursorPos,
		(BYTE*)Callbacks::CenterCursorHooked, 
		"CenterCursor")
		.gateway;

	Engine::CG_DrawBulletImpacts = (CG_DrawBulletImpacts_t)g_Hookman.Trampoline(
		(BYTE*)Engine::CG_DrawBulletImpacts,
		(BYTE*)Callbacks::CG_DrawBulletImpactsHooked,
		"CG_DrawBulletImpacts")
		.gateway;

	Engine::RenderScene = (R_RenderScene)g_Hookman.Trampoline(
		(BYTE*)Engine::RenderScene,
		(BYTE*)Callbacks::RenderSceneHooked,
		"RenderScene")
		.gateway;

	Engine::CG_Recoil = (CG_Recoil_t)g_Hookman.Trampoline(
		(BYTE*)Engine::CG_Recoil,
		(BYTE*)Callbacks::CG_RecoilHooked,
		"CG_Recoil")
		.gateway;

	// add a hook to allow the cursor to move while in game (the game tries to always center the cursor) 


	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(50);
	}

	// restore, clean up, and exit
	SetWindowLongPtr(g_HWND, GWL_WNDPROC, (LONG_PTR)_WndProc);

	g_Hookman.UninstallAllDetours();
	//DxHook::CleanUpAndShutdown(); // implement later
	// this releases the d3dDevice, which we dont want because the game is supposed to keep running
	// instead use ImGui_ImplDX9_InvalidateDeviceObjects();
	//ImGui_ImplDX9_Shutdown(); 

	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplWin32_Shutdown(); // this is fine because it just sets IMGUI's copy of the HWND to 0;
	ImGui::DestroyContext(g_imguiCtx);
	FreeLibraryAndExitThread(DLL, 0);
}


HRESULT __stdcall _EndScene(IDirect3DDevice9* pDevice)
{
	static bool init = true;
	static bool bShowFrameGraph = false;
	static ImFont* pFontConsolas = nullptr;
	auto forward = [](ImGuiConsole<CBuf_AddText_t, int>* self) -> int {
		self->func(0, self->GetCommand());
		return 0;
	};
	static ImGuiConsole<CBuf_AddText_t, int> console(Engine::CBuf_AddText, forward);
	if (init)
	{
		init = false;
		g_imguiCtx = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		pFontConsolas = io.Fonts->AddFontFromFileTTF("C:/WINDOWS/FONTS/CONSOLA.TTF", 14.0);
		

		ImGui_ImplWin32_Init(g_HWND);
		ImGui_ImplDX9_Init(pDevice);

		//SetStyle();
	}

	// prepare to draw

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// set the font to consolas
	ImGui::PushFont(pFontConsolas);

	// draw here
	// main Window
	
	if (ImGui::Begin("Main Window"))
	{
		ImGui::Checkbox("Draw FPS", &bShowFrameGraph);
		//ImGui::InputText("console>", consoleBuf, 256); //console
		static std::pair<std::string, std::string> currentItem = {};
		if (ImGui::BeginCombo("Map: ", currentItem.first.c_str()))
		{
			for (auto _map : maps)
			{
				bool is_selected = currentItem.first == _map.first;
				if (ImGui::Selectable(_map.first.c_str(), is_selected))
				{
					currentItem = _map;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Launch"))
		{
			Engine::CBuf_AddText(0, (char*)currentItem.second.c_str());
		}

		if (ImGui::TreeNode("Hack Options"))
		{
			ImGui::Columns(2);
			if (ImGui::Checkbox("God Mode", &Callbacks::god))
			{
				Engine::CBuf_AddText(0, (char*)"god");
			}

			ImGui::Checkbox("No Recoil", &Callbacks::norecoil);
			ImGui::Checkbox("Rapid Fire", &Callbacks::rapidfire);

			ImGui::NextColumn();

			ImGui::Checkbox("Aimbot", &Callbacks::aimbot);
			ImGui::Checkbox("ESP", &Callbacks::esp);

			if (ImGui::Checkbox("Unlimited Ammo", &Callbacks::ammo))
			{
				// TODO: implement unlimited ammo
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Gameplay Settings"))
		{
			BYTE* cg_fov_struct = *(BYTE**)0x02FF6888;
			ImGui::SliderFloat("cg_fov", (float*)(cg_fov_struct + 0x18), 0, 180, "%.1f");
			ImGui::TreePop();
		}

		if (auto cmd = console.Draw("Console", 0))
		{
			Engine::CBuf_AddText(0, *cmd);
		}

	}
	ImGui::End();

	if (ImGui::Begin("Log"))
	{
		std::string hookDebugInfo = fmt::format("==DirectX Hooks==\n{1}\n\n==game hooks==\n\t{0}",
			g_Hookman.DebugInfo(),
			DxHook::Debug());
		ImGui::Text(hookDebugInfo.c_str());
		ImGui::SetScrollHereY(1.0);
	}
	ImGui::End();

	ImGui::PopFont();

	// FPS graph
	if (bShowFrameGraph)
	{
		frames.push(ImGui::GetIO().Framerate);
		ImGui::SetNextWindowBgAlpha(.2f);

		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Frame Graph", 0, ImGuiWindowFlags_NoDecoration); // begin new window
		ImGui::TextColored(ImVec4(1.0, 0.019, 0.725, 1.0), "FPS: %.0f", frames.peek());
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4());
		ImGui::PlotLines("Framerate", frames.data(), frames.size(), frames.tail(), nullptr, 0.f, 300.f, ImVec2(300.0f, 100.0f));
		ImGui::PopStyleColor();
		ImGui::End();
	}

	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	return endscene(pDevice); // Call original ensdcene so the game can draw
}

HRESULT __stdcall _Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	
	//DebugPrint("Reset called\n");
	HRESULT result = reset(pDevice, pPresentationParameters);
	if(result == D3D_OK)
		ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

LRESULT __stdcall ImguiWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(_WndProc, hWnd, uMsg, wParam, lParam);
}


