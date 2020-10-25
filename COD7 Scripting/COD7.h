#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include "./Utility/VectorMath.h"

extern bool g_god;
extern bool g_norecoil;
extern bool g_rapidfire;
extern bool g_aimbot;
extern bool g_esp;
extern bool g_ammo;

class entityClass
{
public:
	__int32 N00000165; //0x0000
	void* TagInfo; //0x0004
	char pad_0008[4]; //0x0008
	__int32 entityState; //0x000C
	char pad_0010[8]; //0x0010
	vec3_t origin; //0x0018
	char pad_0024[48]; //0x0024
	__int32 entNum; //0x0054
	char pad_0058[184]; //0x0058
	vec3_t headBonePos; //0x0110
	vec3_t CORRECTorigin; //0x011C
	char pad_0128[92]; //0x0128
	__int32 health; //0x0184
	__int32 maxhealth; //0x0188
	char pad_018C[8]; //0x018C
}; //Size: 0x0194

// spawn functions
////typedef int(__cdecl* G_Spawn_t)();
//using CG_CallSpawn_t = int*; // cant guess type ; // 0x488070L
//using G_Callspawn_t = void (__cdecl*) (char* classname); // 0x517580L
//using G_Spawn_t = gentity_t* (__cdecl* )(); // 0x4bf260L
//using G_CallSpawnEntity_t = signed int (__cdecl*) (gentity_t* entptr); // 0x5001f0L
//using Gscr_Spawn_t = int (__cdecl*) (); // 0x7f15c0L
//using SetModelInternal_t = void (__cdecl*)(gentity_t* ent, const char* modelName);
//using G_SetModel_t = void(__cdecl*)(gentity_t* ent, const char* modelName);
//
//// spawn callbacks
//using Scr_SpawnTriggerRadius_t = int __cdecl* (int, int); // 0x42cde0L
//using Scr_Spawntrigger_hurt_t = int __cdecl* (int); // 0x4308f0L
//using Scr_SpawnInfoPlayerRespawn_t = int __cdecl* (int); // 0x436540L
//using Scr_Spawninfo_grenade_hint_t = int __cdecl* (void*); // 0x441ff0L
//using Scr_SpawnScriptModel_t = int __cdecl* (int); // 0x442730L
//using Scr_Spawnlight_t = int __cdecl* (void*, int); // 0x49fc80L
//using Scr_SpawnTriggerDamage_t = int __cdecl* (int, int); // 0x4b8bf0L
//using Scr_SpawnTriggerRadiusUse_t = int __cdecl* (int); // 0x4bd770L
//using Scr_Spawnscript_brushmodel_t = int __cdecl* (void*); // 0x4d57f0L
//using Scr_Spawntrigger_once_t = int __cdecl* (void*); // 0x4d7860L
//using Scr_SpawnScriptOrigin_t = int __cdecl* (int); // 0x4e9500L
////using Scr_Spawnmisc_turret_t = ; // cant guess type ; // 0x50fe20L
//using Scr_SpawnInfoVolume_t = int __cdecl* (void*); // 0x51e5a0L
//using Scr_Spawntrigger_ik_playerclip_terrain_t = int __cdecl* (int); // 0x52d2d0L
//using Scr_Spawntrigger_use_touch_t = int __cdecl* (int, int); // 0x53fa00L
//using Scr_Spawntrigger_multiple_t = int __cdecl* (void*, int); // 0x5438f0L
//using Scr_SpawnTriggerLookat_t = int __cdecl* (void*); // 0x55b0d0L
//using Scr_SpawnScriptVehicleCollMap_t = int __cdecl* (int); // 0x58d800L
//using Scr_Spawntrigger_disk_t = int __cdecl* (void*, int); // 0x5b1f90L
//using Scr_SpawnScriptVehicle_t = int __cdecl* (int, int); // 0x5b5760L
//using Scr_SpawnInfoNotNull_t = int __cdecl* (int, int); // 0x5bdb10L
//using Scr_Spawnscript_struct_t = int __cdecl* (void*); // 0x5bf060L
//using Scr_SpawnInfoPlayerStart_t = int __cdecl* (int); // 0x63e460L
//using Scr_Spawnspawn_manager_t = int* (); // 0x651a30L
//using Scr_Spawntrigger_friendlychain_t = ; // cant guess type ; // 0x68fcb0L
//using GetSpawnFunc_t = int __usercall@<eax>* (int strList@<edi>, const char* classnameStr, int size); // 0x812560L
//extern G_Callspawn_t G_Callspawn;
//extern G_Spawn_t G_Spawn;
//extern G_CallSpawnEntity_t G_CallSpawnEntity;
//extern Gscr_Spawn_t Gscr_Spawn;

void CG_DrawBulletImpactsHooked(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe);

int CBuf_AddTextHooked(int clientNum, char* Text);

int R_AddCmdDrawTextHooked(char* Text, int uk1, void* Font, float x, float y, float uk2, float uk3, float uk4, vec4_t* color, int uk5);

void* R_RegisterFontHooked(const char* FontName, int Unknown1);

void RenderSceneHooked(int a1, int a2);

vec3_t* CL_SetViewAnglesHooked(int clientNum, vec3_t* viewangles);

vec3_t* CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin);

BOOL __cdecl CenterCursorHooked();

extern std::unordered_map<std::string, std::string> maps;

void DrawEngineString(char* text, float x, float y, float size, vec4_t* color);

void TestEsp();

void Aimbot();

//void TrySpawn();