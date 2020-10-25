#include "Engine.h"
#include <fmt/core.h>

using namespace Engine;

void Esp()
{
	int count = 0;
	__int32 viewMatrix = 0x00BA6970;

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
				(float*)viewMatrix,
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
