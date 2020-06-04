#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include "VectorMath.h"

extern bool g_god;
extern bool g_norecoil;
extern bool g_rapidfire;
extern bool g_aimbot;
extern bool g_esp;
extern bool g_ammo;

class centity_t
{
public:
	class entityClass* ent; //0x0000
	char pad_0004[16]; //0x0004
	vec3_t origin; //0x0014
	char pad_0020[84]; //0x0020
	vec3_t origin2; //0x0074
	char pad_0080[12]; //0x0080
}; //Size: 0x008C

class entityList
{
public:
	class centity_t g_entities[64]; //0x0000
}; //Size: 0x2300

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


class refdef_t
{
public:
	__int32 x; //0x0000
	__int32 y; //0x0004
	__int32 width; //0x0008
	__int32 height; //0x000C
	char pad_0010[8]; //0x0010
	float fov_x; //0x0018
	float fov_y; //0x001C
	vec3_t viewOrg; //0x0020
	char pad_002C[8]; //0x002C
	vec3_t viewAxis[3]; //0x0034
	__int32 time; //0x0058
}; //Size: 0x005C

class cg_t
{
public:
	char pad_0000[48]; //0x0000
	class snapshot_t* snap; //0x0030
	class snapshot_t* nextSnap; //0x0034
	__int32 activeSnapshots; //0x0038
	char N00000052[143409][4]; //0x003C
	class refdef_t refdef; //0x8C100
}; //Size: 0x8C15C

class CLocalPlayer
{
public:
	char pad_0000[36]; //0x0000
	vec3_t worldPosition; //0x0024
	vec3_t momentum; //0x0030
	__int32 actionTimer; //0x003C
	__int32 animationTimer; //0x0040
	char pad_0044[52]; //0x0044
	__int32 speed; //0x0078
	char pad_007C[4]; //0x007C
	__int32 gravity; //0x0080
	char pad_0084[192]; //0x0084
	__int32 currentWeapon; //0x0144
	char pad_0148[56]; //0x0148
	vec2_t viewAngles; //0x0180
	char pad_0188[60]; //0x0188
	__int32 health; //0x01C4
	char pad_01C8[4]; //0x01C8
	__int32 maxHeatlh; //0x01CC
	char pad_01D0[372]; //0x01D0
	__int32 primaryWeapIdx; //0x0344
	__int32 spareAmmo; //0x0348
	char pad_034C[8]; //0x034C
	__int32 secondaryWeapIdx; //0x0354
	__int32 spareAmmo2; //0x0358
	char pad_035C[96]; //0x035C
	__int32 primaryWeaponIndex; //0x03BC
	__int32 primaryClipAmmo; //0x03C0
	char pad_03C4[4]; //0x03C4
	__int32 grenadeAmmo; //0x03C8
	__int32 secondaryWeaponIndex; //0x03CC
	__int32 secondaryClipAmmo; //0x03D0
	char pad_03D4[788]; //0x03D4
}; //Size: 0x06E8

class weaponDef_t
{
public:
	char* weaponName; //0x0000
	char pad_0004[8]; //0x0004
	char* weapon_SUPERNAME; //0x000C
	char pad_0010[16]; //0x0010
	__int32 maxClipSize; //0x0020
	char pad_0024[16]; //0x0024
	__int32 maybeADamageStat; //0x0034
	__int32 maybeADamageStat2; //0x0038
	__int32 maybeACriticalDamageStat; //0x003C
	char pad_0040[4]; //0x0040
	__int32 weaponIdx; //0x0044
	char pad_0048[4]; //0x0048
	__int32 weaponIdx2; //0x004C
	char pad_0050[148]; //0x0050
}; //Size: 0x00E4

class weapons
{
public:
	class weaponDef_t* weapon_array[92]; //0x0000
	char pad_0170[4156]; //0x0170
}; //Size: 0x11AC

struct trace_t
{
	char padding[16];
	/* 0x0010 */ float fraction;
	/* 0x0014 */ int sflags;
	/* 0x0018 */ int cflags;
	/* 0x001c */ int hitType;
	/* 0x0020 */ unsigned short hitId;
	/* 0x0022 */ unsigned short modelIndex;
	/* 0x0024 */ unsigned short partName;
	/* 0x0026 */ unsigned short boneIndex;
	/* 0x0028 */ unsigned short partGroup;
	/* 0x002a */ bool allsolid;
	/* 0x002b */ bool startsolid;
	/* 0x002c */ bool walkable;
	/* 0x002d */ char Padding_455[3];
	/* 0x0030 */ struct cStaticModel_s* staticModel;
	/* 0x0034 */ int hitPartition;
};

//typedef 
typedef void(__cdecl* CG_DrawBulletImpacts_t)(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe);

typedef int(__cdecl* CBuf_AddText_t)(int clientNum, char* Text);

typedef int(*R_AddCmdDrawText_t)(char* Text, int uk1, void* Font, float x, float y, float uk2, float uk3, float uk4, vec4_t* color, int uk5);

typedef void* (*RegisterFont_)(const char* FontName, int Unknown1);

typedef void(__cdecl* R_RenderScene)(int a1, int a2);

typedef vec3_t* (__cdecl* CL_SetViewAngles_t)(int clientNum, vec3_t* viewangles);

typedef char(__cdecl* CG_Trace_t)(trace_t* trace, vec3_t* start, vec3_t* mins, vec3_t* maxs, vec3_t* end, int skip, DWORD mask, vec3_t* vNull);

typedef vec3_t* (__cdecl* CG_Recoil_t)(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin);


extern CG_Recoil_t CG_Recoil;
extern CG_DrawBulletImpacts_t CG_DrawBulletImpacts;
extern CBuf_AddText_t CBuf_AddText;
extern R_AddCmdDrawText_t R_AddCmdDrawText;
extern RegisterFont_ R_RegisterFont;
extern R_RenderScene RenderScene;
extern CL_SetViewAngles_t CL_SetViewAngles;
extern CG_Trace_t CG_Trace;

void CG_DrawBulletImpactsHooked(int localClientNum, void* entityOrigin, unsigned __int16 rand, void* playerState_s, int weapID, int EVENT_ID, bool bADS_maybe);

int CBuf_AddTextHooked(int clientNum, char* Text);

int R_AddCmdDrawTextHooked(char* Text, int uk1, void* Font, float x, float y, float uk2, float uk3, float uk4, vec4_t* color, int uk5);

void* R_RegisterFontHooked(const char* FontName, int Unknown1);

void RenderSceneHooked(int a1, int a2);

vec3_t* CL_SetViewAnglesHooked(int clientNum, vec3_t* viewangles);

char CG_TraceHooked(trace_t* trace, vec3_t* start, vec3_t* mins, vec3_t* maxs, vec3_t* end, int skip, DWORD mask, vec3_t* vNull);

vec3_t* CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin);


extern std::unordered_map<std::string, std::string> maps;

void DrawEngineString(char* text, float x, float y, float size, vec4_t* color);

//bool WorldToScreen(vec3_t src, vec3_t dst, vec3_t& screen, float fovx, float fovy, float windowWidth, float windowHeight, vec3_t left, vec3_t up, vec3_t forward);

bool WorldToScreen(refdef_t* refdef, vec3_t dst, vec3_t& screen);

void TestEsp();

vec2_t CalcAngles(vec3_t& src, vec3_t& dst, vec3_t viewAxis[3]);

void Aimbot();

