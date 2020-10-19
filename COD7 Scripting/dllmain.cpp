#include "common.hpp"
#include "utils.hpp"
#include "Engine.hpp"
#include "COD7.h"
#include "ImGuiConsole.h"
// prototypes
void MainThread(HMODULE DLL);
HRESULT __stdcall _EndScene(IDirect3DDevice9* pDevice);
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

void MainThread(HMODULE DLL)
{
	// setup
	g_HWND = GetProcessHwnd();
	_WndProc = (WNDPROC)GetWindowLongPtr(g_HWND, GWL_WNDPROC);
	SetWindowLongPtr(g_HWND, GWL_WNDPROC, (LONG_PTR)ImguiWndProc);

	endscene = DxHook::Hook<EndScene_t>(D3DVTABLE_INDEX::iEndScene, _EndScene);

	// install hack hooks
	HookManager hookman;
	CenterCursorPos = (CenterCursorPos_t)hookman.Trampoline((BYTE*)CenterCursorPos, (BYTE*)CenterCursorHooked).gateway;
	PrintError = (PrintError_t)hookman.Trampoline((BYTE*)0x0043BF30, (BYTE*)LogPrintMsg).gateway;
	CG_DrawBulletImpacts = (CG_DrawBulletImpacts_t)hookman.Trampoline((BYTE*)CG_DrawBulletImpacts, (BYTE*)CG_DrawBulletImpactsHooked).gateway;
	RenderScene = (R_RenderScene)hookman.Trampoline((BYTE*)RenderScene, (BYTE*)RenderSceneHooked).gateway;
	CG_Recoil = (CG_Recoil_t)hookman.Trampoline((BYTE*)CG_Recoil, (BYTE*)CG_RecoilHooked).gateway;

	// add a hook to allow the cursor to move while in game (the game tries to always center the cursor) 


	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(50);
	}

	// restore, clean up, and exit
	SetWindowLongPtr(g_HWND, GWL_WNDPROC, (LONG_PTR)_WndProc);

	hookman.UninstallAllDetours();
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(g_imguiCtx);
	FreeLibraryAndExitThread(DLL, 0);
}

auto forward = [](ImGuiConsole<CBuf_AddText_t, int>* self) -> int {
	self->func(0, self->GetCommand());
	return 0;
};

HRESULT __stdcall _EndScene(IDirect3DDevice9* pDevice)
{
	static bool init = true;
	static bool bShowFrameGraph = false;
	static ImGuiConsole<CBuf_AddText_t, int> console(CBuf_AddText, forward);
	if (init)
	{
		init = false;
		g_imguiCtx = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		ImGui_ImplWin32_Init(g_HWND);
		ImGui_ImplDX9_Init(pDevice);

		//SetStyle();
	}

	// prepare to draw

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


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
			CBuf_AddText(0, (char*)currentItem.second.c_str());
		}

		if (ImGui::TreeNode("Hack Options"))
		{
			ImGui::Columns(2);
			if (ImGui::Checkbox("God Mode", &g_god))
			{
				CBuf_AddText(0, (char*)"god");
			}

			ImGui::Checkbox("No Recoil", &g_norecoil);
			ImGui::Checkbox("Rapid Fire", &g_rapidfire);

			ImGui::NextColumn();

			ImGui::Checkbox("Aimbot", &g_aimbot);
			ImGui::Checkbox("ESP", &g_esp);

			if (ImGui::Checkbox("Unlimited Ammo", &g_ammo))
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
			CBuf_AddText(0, *cmd);
		}

	}
	ImGui::End();

	if (ImGui::Begin("Log"))
	{
		ImGui::Text(g_errors.str().data());
		ImGui::SetScrollHereY(1.0);
	}
	ImGui::End();



	// FPS graph
	if (bShowFrameGraph)
	{
		frames.push(ImGui::GetIO().Framerate);
		ImGui::SetNextWindowBgAlpha(.2);

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

LRESULT __stdcall ImguiWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(_WndProc, hWnd, uMsg, wParam, lParam);
}


