#define _USE_MATH_DEFINES
#include <cmath>
#include "VectorMath.h"


float DotProduct(vec3_t& v1, vec3_t& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void VectorSubtract(vec3_t& v1, vec3_t& v2, vec3_t& result)
{
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
}

void VectorAdd(vec3_t& v1, vec3_t& v2, vec3_t& out)
{
	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	out.z = v1.z + v2.z;
}

void VectorCopy(vec3_t& src, vec3_t& dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void VectorScale(vec3_t& v1, float scale, vec3_t& out)
{
	out.x = v1.x * scale;
	out.y = v1.y * scale;
	out.z = v1.z * scale;
}

void VectorMultiply(vec3_t& v1, vec3_t& v2, vec3_t& normalized)
{
	normalized.x = v1.x * v2.x;
	normalized.y = v1.y * v2.y;
	normalized.z = v1.z * v2.z;
}

void VectorMultilpyAdd(vec3_t& v1, float scale, vec3_t& add, vec3_t& out)
{
	out.x = v1.x + add.x * scale;
	out.y = v1.y + add.y * scale;
	out.z = v1.z + add.z * scale;
}

float VectorMagnitude(vec3_t& v1)
{
	return sqrtf(powf(v1.x, 2.f) + powf(v1.y, 2.f) + powf(v1.z, 2.f));
}

void VectorNormalize(vec3_t& v1, vec3_t& out)
{
	float length = VectorMagnitude(v1);
	out.x = v1.x / length;
	out.y = v1.y / length;
	out.z = v1.z / length;
}



/* Converts Angles to vectors */
void AngleVectors(const vec3_t angles, vec3_t* forward, vec3_t* right, vec3_t* up) {
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	// YAW		= 1 = y
	// PITCH	= 0 = x
	// ROLL		= 2 = z
	angle = angles.y * (M_PI * 2 / 360.f); // yaw
	sy = sin(angle);
	cy = cos(angle);
	angle = angles.x * (M_PI * 2 / 360.f); // pitch
	sp = sin(angle);
	cp = cos(angle);
	angle = angles.z * (M_PI * 2 / 360.f); // roll
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}
	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}


