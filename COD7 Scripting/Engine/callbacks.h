#pragma once

namespace Callbacks 
{
	extern bool god;
	extern bool norecoil;
	extern bool rapidfire;
	extern bool aimbot;
	extern bool esp;
	extern bool ammo;

	bool __cdecl CenterCursorHooked();
	vec3_t* CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin);
	void CG_DrawBulletImpactsHooked(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe);
	void RenderSceneHooked(int a1, int a2);

};