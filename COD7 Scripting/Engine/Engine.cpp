#include "Engine.h"
#include "../Utility/VectorMath.h"
#include <cstring>


using namespace Engine;

std::unordered_map<std::string, std::string> maps = {
	{"Kino der Toten",		"devmap zombie_theater"},
	{"Five",				"devmap zombie_pentagon"},
	{"Dead Ops Arcade",		"devmap zombietron"},
	{"Nacht Der Untoten",	"devmap zombie_cod5_prototype"},
	{"Verruckt",			"devmap zombie_cod5_asylum"},
	{"Shi No Numa",			"devmap zombie_cod5_sumpf"},
	{"Der Riese",			"devmap zombie_cod5_factory"},
	{"Ascension",			"devmap zombie_cosmodrome"},
	{"Call of the Dead",	"devmap zombie_coast"},
	{"Shangri-La",			"devmap zombie_temple"},
	{"Moon",				"devmap zombie_moon"}
};

// Function instances
CG_DrawBulletImpacts_t	Engine::CG_DrawBulletImpacts=  (CG_DrawBulletImpacts_t)0x799a90;
CG_Trace_t				Engine::CG_Trace			= (CG_Trace_t)0x68fd90L;
R_AddCmdDrawText_t		Engine::R_AddCmdDrawText	= (R_AddCmdDrawText_t)0x6d6460L;
CG_Recoil_t				Engine::CG_Recoil			= (CG_Recoil_t)0x05676C0;
CG_GetEntity_t			Engine::CG_GetEntity		= (CG_GetEntity_t)0x00405D70;
CL_SetViewAngles_t		Engine::CL_SetViewAngles	= (CL_SetViewAngles_t)0x60adf0L;
CG_RegisterTag_t		Engine::CG_RegisterTag		= (CG_RegisterTag_t)0x0055ABF0;
RegisterFont_			Engine::R_RegisterFont		= (RegisterFont_)0x004DD5F0;
CBuf_AddText_t			Engine::CBuf_AddText		= (CBuf_AddText_t)0x49b930L;
R_RenderScene			Engine::RenderScene			= (R_RenderScene)0x6C8CD0;
CenterCursorPos_t		Engine::CenterCursorPos		= (CenterCursorPos_t)0x5b7700L;

// Structure instances
cg_t* Engine::cg = (cg_t*)0x29C69430;
CLocalPlayer* Engine::localplayer = (CLocalPlayer*)0x01C08B40;
//entityList* g_entities = (entityList*)0x1BFBC84;


bool WorldToScreenMatrix(vec3_t pos, vec2_t& screen, float matrix[16], int windowWidth, int windowHeight)
{
	vec4_t clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;


	vec3_t NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

bool GetTagPos(void* ent, const char* tag, vec3_t* vOut) 
{
	//signed int __usercall AimTarget_GetTagPos@<eax>(signed int result@<eax>, struct_Ent *entity@<esi>, int a3, Vector3 *outVec)
	int AimTarget_GetTagPos = 0x007562C0;
	__int16 j_Head = CG_RegisterTag(tag, 0); // 93

	__asm
	{
		push vOut

		xor edx, edx
		push edx

		mov esi, ent

		movzx edi, j_Head

		xor eax, eax

		call AimTarget_GetTagPos
		add esp, 8 // we pushed 2 4 byte values

	}
	return true;
}

void DrawEngineString(char* text, float x, float y, float size, vec4_t* color)
{
	static void* dwFont = R_RegisterFont("fonts/normalfont", 1);
	R_AddCmdDrawText(text, strlen(text), dwFont, x, y, size, size, 0, color, 0);
}
