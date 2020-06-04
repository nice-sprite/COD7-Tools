#pragma once
struct vec4_t
{
	float x, y, z, w;
};

typedef vec4_t RBGA;

struct vec3_t
{
	float x, y, z;
};


struct vec2_t
{
	float x, y;
};



float DotProduct(vec3_t& v1, vec3_t& v2);

void VectorSubtract(vec3_t& v1, vec3_t& v2, vec3_t& result);

void VectorAdd(vec3_t& v1, vec3_t& v2, vec3_t& out);

void VectorCopy(vec3_t& src, vec3_t& dst);

void VectorScale(vec3_t& v1, float scale, vec3_t& out);

void VectorMultiply(vec3_t& v1, vec3_t& v2, vec3_t& out);

void VectorMultilpyAdd(vec3_t& v1, float scale, vec3_t& add, vec3_t& out);

float VectorMagnitude(vec3_t& v1);

void VectorNormalize(vec3_t& v1, vec3_t& out);

void AngleVectors(const vec3_t angles, vec3_t* forward, vec3_t* right, vec3_t* up);


//DotProduct(x, y)
//VectorSubtract(a, b, c)
//VectorAdd(a, b, c)
//VectorCopy(a, b)
//VectorScale(v, s, o)
//VectorMA(v, s, b, o)