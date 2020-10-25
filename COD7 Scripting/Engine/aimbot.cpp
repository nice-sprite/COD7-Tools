#include "aimbot.h"

using namespace Engine;

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
