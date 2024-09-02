#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

float vec2_lenght(vec2_t vec);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mult(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
void vec2_normalize(vec2_t *vec);

float vec3_lenght(vec3_t vec);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mult(vec3_t v, float factor);
vec3_t vec3_div(vec3_t v, float factor);
void vec3_normalize(vec3_t *vec);

// return the perpendicular resultant vector of cross product operation between 2 vectors
vec3_t vec3_crossP(vec3_t a, vec3_t b); 

// dot product determines how aligned are the vectors a and b, if the value is 
// more than zero, they are more aligned, if its 0, they are not, if its negative
// they are even more unaligned
float vec3_dotP(vec3_t a, vec3_t b);

vec3_t vec3_rotate_x(vec3_t vec, float angle);
vec3_t vec3_rotate_y(vec3_t vec, float angle);
vec3_t vec3_rotate_z(vec3_t vec, float angle);

#endif // !VECTOR_H
