#include "vector.h"
#include <math.h>

float vec2_lenght(vec2_t vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

float vec3_lenght(vec3_t vec)
{
	return sqrt(vec.x * vec.x * vec.z + vec.y * vec.y * vec.z);
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
	vec2_t res = { a.x + b.x, a.y + b.y };
	return res;
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	vec2_t res = { a.x - b.x, a.y - b.y };
	return res;
}

float vec2_dotP(vec2_t a, vec2_t b)
{
	return (a.x * b.x) + (a.y * b.y);
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
	vec3_t res = { a.x + b.x, a.y + b.y, a.z + b.z };
	return res;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	vec3_t res = { a.x - b.x, a.y - b.y, a.z - b.z };
	return res;
}

vec2_t vec2_mult(vec2_t v, float factor)
{
	vec2_t res = { v.x * factor, v.y * factor };
	return res;
}

vec2_t vec2_div(vec2_t v, float factor)
{
	vec2_t res = { v.x / factor, v.y / factor };
	return res;
}

void vec2_normalize(vec2_t* vec)
{
	float lenght = sqrt(vec->x * vec->x + vec->y * vec->y);
	vec->x /= lenght;
	vec->y /= lenght;
}

vec3_t vec3_mult(vec3_t v, float factor)
{
	vec3_t res = { v.x * factor, v.y * factor, v.z * factor };
	return res;
}

vec3_t vec3_div(vec3_t v, float factor)
{
	vec3_t res = { v.x / factor, v.y / factor, v.z / factor };
	return res;
}

vec3_t vec3_crossP(vec3_t a, vec3_t b)
{
	vec3_t res = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return res;
}

float vec3_dotP(vec3_t a, vec3_t b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(vec3_t* vec)
{
	float lenght = sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	vec->x /= lenght;
	vec->y /= lenght;
	vec->z /= lenght;
}

vec3_t vec3_rotate_x(vec3_t vec, float angle)
{
	vec3_t rotated_vector = {
		vec.x,
		vec.y * cos(angle) - vec.z * sin(angle),
		vec.y * sin(angle) + vec.z * cos(angle)
	};
	return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t vec, float angle)
{
	vec3_t rotated_vector = {
		vec.x * cos(angle) - vec.z * sin(angle),
		vec.y,
		vec.x * sin(angle) + vec.z * cos(angle)
	};
	return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t vec, float angle)
{
	vec3_t rotated_vector = {
		vec.x * cos(angle) - vec.y * sin(angle),
		vec.x * sin(angle) + vec.y * cos(angle),
		vec.z
	};
	return rotated_vector;
}
