#include "Engine.h"
#include "callbacks.h"
#include "aimbot.h"
#include "sight.h"
#include <Windows.h>

using namespace Engine;

bool Callbacks::god = false;
bool Callbacks::norecoil = false;
bool Callbacks::rapidfire = false;
bool Callbacks::aimbot = false;
bool Callbacks::esp = false;
bool Callbacks::ammo = false;

bool __cdecl Callbacks::CenterCursorHooked()
{
	// changing these globals actually make the function not be called at all anymore
	// once they have been set to false, they cannot be unset from this function
	// temp work-around: we dont check the keystate in the same function as we set the variables with
	// keystate polling temporarily moved to RenderSceneHooked
	//TODO implement keyboard hook instead of polling per-frame
	static int** in_mouse = (int**)0x0276C098;
	static bool* b_in_mouse = (bool*)0x0276C0C1;

	if (*b_in_mouse || *((*in_mouse) + 0x18))
	{
		return CenterCursorPos();
	}
}

//TODO no global state variables
vec3_t* Callbacks::CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin)
{
	// uses global state variable
	if (norecoil) return viewAngles;
	return CG_Recoil(cgameinfo, viewAngles, origin);
}

void Callbacks::CG_DrawBulletImpactsHooked(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe)
{
	return CG_DrawBulletImpacts(localClientNum,
		entityOrigin,
		rand,
		playerState_s,
		weapID,
		EVENT_ID,
		bADS_maybe
	);
}

//TODO no global state variables
void Callbacks::RenderSceneHooked(int a1, int a2)
{
	if (esp) Esp();

	if (aimbot) Aimbot();

	//temp fix for mouse input issue described in CenterCursorHooked()
	static int** in_mouse = (int**)0x0276C098;
	static bool* b_in_mouse = (bool*)0x0276C0C1;

	if (GetAsyncKeyState(VK_MENU)) {
		*((*in_mouse) + 0x18) = false;
		*b_in_mouse = false;
	}
	else {
		*((*in_mouse) + 0x18) = true;
		*b_in_mouse = true;
	}

	return RenderScene(a1, a2);
}

