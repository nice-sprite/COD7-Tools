#define _USE_MATH_DEFINES // for C++
#include "COD7.h"
#include "VectorMath.h"
#include <cmath>
#include <cstring>
#include <cfloat> // for max float value

bool g_god = false;
bool g_norecoil = false;
bool g_rapidfire = false;
bool g_aimbot = false;
bool g_esp = false;
extern bool g_ammo = false;

// Game function definitions/locations
CG_DrawBulletImpacts_t	CG_DrawBulletImpacts = (CG_DrawBulletImpacts_t)0x799a90;
CBuf_AddText_t			CBuf_AddText = (CBuf_AddText_t)0x49b930L;
R_AddCmdDrawText_t		R_AddCmdDrawText = (R_AddCmdDrawText_t)0x6d6460L;
RegisterFont_			R_RegisterFont = (RegisterFont_)0x004DD5F0;/*0x6d3010L;*/
R_RenderScene			RenderScene = (R_RenderScene)0x6C8CD0/*0x629770L*/;
CL_SetViewAngles_t		CL_SetViewAngles = (CL_SetViewAngles_t)0x60adf0L;
CG_Trace_t				CG_Trace = (CG_Trace_t)0x68fd90L;
CG_Recoil_t				CG_Recoil = (CG_Recoil_t)0x05676C0;

// Game structures
entityList* g_entities = (entityList*)0x1BFBC84;
cg_t* cg = (cg_t*)0x29C69430;
CLocalPlayer* localplayer = (CLocalPlayer*)0x01C08B40;
weapons* weapon = (weapons*)0x0BE19A8;

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
void CG_GetTagPos(void* ent, void* DObj, int tag, vec3_t* outvec)
{
	__asm
	{
		push outvec
		push tag
		push DObj
		push ent
		mov eax, 0x44A0A0
		call eax
		add esp, 0x10
	}
}

PVOID GetDObj(UINT clientnum, void* a2)
{
	__asm
	{
		push a2
		push clientnum
		mov eax, 0x501850
		call eax
		add esp, 0x8
	}
}

bool GetTagPosition(entityClass* ent, int Tag, vec3_t* vOut)
{
	PVOID DObj = GetDObj(ent->entNum, (void*)ent->TagInfo); // TagInfo == *( int *)( ent + 0x4 )

	if (!DObj)
	{
		return false;
	}
	CG_GetTagPos((void*)ent, DObj, Tag, vOut);
	return true;
}

void DrawEngineString(char* text, float x, float y, float size, vec4_t* color)
{
	//void* dwFont = *(void**)0x02576148;
	//void* dwFont = R_RegisterFont("fonts/consoleFont", 0);
	static void* dwFont = R_RegisterFont("fonts/normalfont", 1);

	//color = *(float**)0x0A5E794;
	R_AddCmdDrawText(text, strlen(text), dwFont, x, y, size, size, 0, color, 0);
	//R_AddCmdDrawText(text, strlen(text), dwFont, x, y + 50, size, size, 0, color, 0);

}

bool WorldToScreen(refdef_t* refdef, vec3_t dst, vec3_t& screen)
{
	vec3_t transform;
	float xCenter = refdef->width * .5f, yCenter = refdef->height * .5f;

	static float fovy = 51.077193f;
	static float fovx = 80.690943f;

	static float px = (float)tan(fovx * M_PI / 360.0f);
	static float py = (float)tan(fovy * M_PI / 360.0f);
	float z = 0.f;

	//VectorSubtract((float*)&dst, (float*)&refdef->viewOrg, (float*)&transform);
	VectorSubtract(dst, refdef->viewOrg, transform);

	z = DotProduct(transform, refdef->viewAxis[0]);
	if (z <= .1f) return false;
	screen.x = xCenter - DotProduct(transform, refdef->viewAxis[1]) * xCenter / (z * px);
	screen.y = yCenter - DotProduct(transform, refdef->viewAxis[2]) * yCenter / (z * py);
	return true;

}

void TestEsp()
{
	vec3_t screenPos{};
	vec3_t entityOrigin;
	for (int i = 0; i < 64; i++)
	{
		if (g_entities->g_entities[i].ent != nullptr)
		{
			entityOrigin = g_entities->g_entities[i].ent->CORRECTorigin;
			//correct for head pos
			vec3_t head = { 0.0f, 0.0f, 50.f };
			VectorAdd(entityOrigin, head, entityOrigin);
		}
		else continue;

		bool onScreen = WorldToScreen(&cg->refdef, entityOrigin, screenPos);

		if (onScreen && g_entities->g_entities[i].ent->health > 0)
		{
			vec4_t color = { 1.0, 0.f, 0.f, 1.0 };
			trace_t trace;
			vec3_t zero = {};
			CG_Trace(&trace, &cg->refdef.viewOrg, &zero, &zero, &entityOrigin, 0, 0x803003, &zero);
			if (trace.fraction >= .97f)
			{
				color = { .5f, 0.f, 1.f, 1.0 };
			}
			DrawEngineString((char*)".\0", screenPos.x, screenPos.y, 1.0f, &color);
		}

	}
}

float Radians(float degrees) { return (M_PI / 180.f) * degrees; }

vec2_t CalcAngles(vec3_t& src, vec3_t& dst, vec3_t viewAxis[3])
{
	vec3_t deltaVec, normalized;
	VectorSubtract(dst, src, deltaVec);
	//VectorNormalize(deltaVec, deltaVec);
	float magnitude = VectorMagnitude(deltaVec);
	VectorNormalize(deltaVec, normalized);
	float yaw = -atan2(normalized.x, normalized.y) * (180.f / M_PI) + 30.0f;

	float pitch = atan2(deltaVec.z, magnitude) * (180.f / M_PI); // x
	//VectorNormalize(deltaVec, normalized);

	//VectorMultiply(viewAxis[1], normalized, deltaVec); // yaw
	//float yaw = asin(deltaVec.x + deltaVec.y + deltaVec.z) * (180.f / M_PI);

	//VectorMultiply(viewAxis[2], normalized, deltaVec); // pitch
	//float pitch = -asin(Radians(deltaVec.x + deltaVec.y + deltaVec.z)) * (180.f / M_PI);

	return vec2_t{ yaw, pitch };
}

void Aimbot()
{
	if (!GetAsyncKeyState(VK_RBUTTON))
		return;

	vec3_t entityOrigin;
	vec3_t angles = {};
	float dist = FLT_MAX;
	vec3_t deltaVec;
	int closestEnt = -1;
	vec3_t* currentAngles = (vec3_t*)0x2911E20;
	for (int i = 0; i < 64; i++)
	{
		if (g_entities->g_entities[i].ent != nullptr)
		{
			entityOrigin = g_entities->g_entities[i].ent->CORRECTorigin;
			//GetTagPosition(g_entities->g_entities[i].ent, 1, &entityOrigin);
			if (entityOrigin.x == 0 && entityOrigin.y == 0 && entityOrigin.z == 0)
				continue;
			VectorSubtract(entityOrigin, cg->refdef.viewOrg, deltaVec);
			float mag = VectorMagnitude(deltaVec);
			if (mag < dist && g_entities->g_entities[i].ent->health > 0)
			{
				dist = mag;
				closestEnt = i;
			}
		}
	}
	if (closestEnt >= 0)
	{
		entityOrigin = g_entities->g_entities[closestEnt].ent->CORRECTorigin;
		vec3_t adjust{ 0.f, 0.f, 45.f };
		VectorAdd(entityOrigin, adjust, entityOrigin);
		vec2_t v = CalcAngles(cg->refdef.viewOrg, entityOrigin, cg->refdef.viewAxis);
		currentAngles->x = -v.y; // pitch
		currentAngles->y = v.x; // yaw
	}
	//for (int i = 1; i < 32; i++)
	//{
	//	if (g_entities->g_entities[i].ent != nullptr)
	//	{

	//		entityOrigin = g_entities->g_entities[i].ent->CORRECTorigin;
	//		//correct for head pos
	//		vec3_t head = { 0.0f, 0.0f, 50.f };
	//		VectorAdd(entityOrigin, head, entityOrigin);
	//	}
	//	else continue;

	//	if (g_entities->g_entities[i].ent->health > 0)
	//	{
	//		vec2_t v = CalcAngles(cg->refdef.viewOrg, entityOrigin, cg->refdef.viewAxis);

	//		angles = { v.x, v.y, 0.f };


	//		//CL_SetViewAngles(0, &aimAt);
	//	}
	//}



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
	if (g_esp) TestEsp();
	if (g_aimbot) Aimbot();
	return RenderScene(a1, a2);
}

vec3_t* CL_SetViewAnglesHooked(int clientNum, vec3_t* viewangles)
{
	return viewangles;
}

char CG_TraceHooked(trace_t* trace, vec3_t* start, vec3_t* mins, vec3_t* maxs, vec3_t* end, int skip, DWORD mask, vec3_t* vNull)
{
	return true;
}

vec3_t* CG_RecoilHooked(int* cgameinfo, vec3_t* viewAngles, vec3_t* origin)
{
	if (g_norecoil) return viewAngles;
	//CBuf_AddText(0, (char*)"say Shooting...");
	return CG_Recoil(cgameinfo, viewAngles, origin);
}
