#define _USE_MATH_DEFINES // for C++
#include "COD7.h"
#include "./Utility/VectorMath.h"
#include "Engine/Engine.hpp"
#include <cmath>
#include <cstring>
#include <cfloat> // for max float value
#include <fmt/core.h>

bool g_god = false;
bool g_norecoil = false;
bool g_rapidfire = false;
bool g_aimbot = false;
bool g_esp = false;
extern bool g_ammo = false;


// spawn funcs
//G_Callspawn_t			G_Callspawn = (G_Callspawn_t)0x517580L;
//G_Spawn_t				G_Spawn = (G_Spawn_t)0x4bf260L;
//G_CallSpawnEntity_t		G_CallSpawnEntity = (G_CallSpawnEntity_t)0x5001f0L;
//Gscr_Spawn_t			Gscr_Spawn = (Gscr_Spawn_t)0x7f15c0L;




// map names with console launch command
std::unordered_map<std::string, std::string> maps = {
	{"Kino der Toten",		"devmap zombie_theater"},
	{"Five",				"devmap zombie_pentagon"},
	{"Dead Ops Arcade",		"devmap zombietron"},
	{"Nacht Der Untoten",	"devmap zombie_cod5_prototype"},
	{"Verruckt",			"devmap zombie_cod5_asylum"},
	{"Shi No Numa",			"devmap zombie_cod5_sumpf"},
	{"Der Riese",			"devmap zombie_cod5_factory"},
	{"Ascension",			"devmap zombie_cosmodrome"},
	{"Call of The Dead",	"devmap zombie_coast"},
	{"Shangri-La",			"devmap zombie_temple"},
	{"Moon",				"devmap zombie_moon"}
};


// Functions

void Aimbot()
{
	if (!GetAsyncKeyState(VK_RBUTTON))
		return;


	_centity_t* closest = nullptr;
	float closestDistance = FLT_MAX;

	for (int i = 0; i < 1024; i++)
	{
		_centity_t* ent = CG_GetEntity(0, i);
		// make sure ent is alive and a zombie
		if (ent && !ent->pose.isRagdoll && ent->pose.eType == 0x10)
		{
			vec3_t head;
			vec3_t currentDist;
			GetTagPos(ent, "j_neck", &head);
			VectorSubtract(head, cg->refdef.viewOrg, currentDist);
			float mag = VectorMagnitude(currentDist);

			if (mag < closestDistance)
			{
				closestDistance = mag;
				closest = ent;
			}
		}
	}


	// by now, we have iterated through and found the closest entity to us, so lets aim at it
	if (closest != nullptr && !closest->pose.isRagdoll)
	{
		vec3_t headPosition = { 0.f, 0.f, 0.f };
		vec3_t* currentAngles = (vec3_t*)0x2911E20;
		vec3_t deltaVec{};
		vec3_t angleVec{};

		GetTagPos(closest, "j_head", &headPosition);
		VectorSubtract(headPosition, cg->refdef.viewOrg, deltaVec);
		VectorToAngles(deltaVec, angleVec);
		currentAngles->x = angleVec.x; // pitch
		currentAngles->y = angleVec.y - 61.5f; // yaw

	}
	

}

void Esp()
{
	int count = 0;
	DWORD viewMatrix = 0x00BA6970;

	for (int i = 0; i < 1024; i++)
	{
		_centity_t* ent = CG_GetEntity(0, i);
		if (ent)
		{
			vec3_t head;
			vec2_t screenPos;
			GetTagPos(ent, "j_neck", &head);
			bool onScreen = WorldToScreenMatrix(
				head,
				screenPos, 
				(FLOAT*)viewMatrix, 
				cg->refdef.width,
				cg->refdef.height
			);

			// checking if ragdoll will work for now, later reverse Cscr_IsAlive to check eFlags
			if (onScreen && !ent->pose.isRagdoll && ent->pose.eType == 0x10)
			{
				vec4_t color = { 1.0, 0.f, 0.5f, 1.0f };
				DrawEngineString((char*)"o\0", screenPos.x, screenPos.y, 1.0f, &color);
				count++;
			}
		}
	}
	vec4_t c = { 1.0, 1.0f, 0.5f, 1.0f };
	auto str = fmt::format("valid ents: {}", count);
	DrawEngineString((char*)str.c_str(), 10, 30, 1.0f, &c);

}

// Hook/Custom versions of Game Functions
void CG_DrawBulletImpactsHooked(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe)
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

int CBuf_AddTextHooked(int clientNum, char* Text)
{
	return 0;
}

int R_AddCmdDrawTextHooked(char* Text, int uk1, void* Font, float x, float y, float uk2, float uk3, float uk4, vec4_t* color, int uk5)
{
	return 0;
}

void* R_RegisterFontHooked(const char* FontName, int Unknown1)
{
	return (void*)0;
}

void RenderSceneHooked(int a1, int a2)
{
	if (g_esp) Esp();

	if (g_aimbot) Aimbot();

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

vec3_t* CL_SetViewAnglesHooked(int clientNum, vec3_t* viewangles)
{
	return viewangles;
}

// TODO move 
vec3_t* CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin)
{
	if (g_norecoil) return viewAngles;
	return CG_Recoil(cgameinfo, viewAngles, origin);
}

BOOL __cdecl CenterCursorHooked()
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

