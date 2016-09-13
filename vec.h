#ifndef GPA_VEC_H_
#define GPA_VEC_H_

typedef struct
{
	int16_t x,y;
} vec2i;


inline int16_t vec2i_length_sq(vec2i* r){
	return (r->x)*(r->x)+(r->y)*(r->y);
}

inline int16_t vec2i_length(vec2i* r){
	return vec2i_length_sq(r);
}

inline vec2i* vec2i_negate(vec2i* r){
	r->x = -r->x;
	r->y = -r->y;
	return r;
}

inline vec2i* vec2i_add(vec2i* l, vec2i* r){
	l->x += r->x;
	l->y += r->y;
	return l;
}

inline vec2i* vec2i_sub(vec2i* l, vec2i* r){
	l->x -= r->x;
	l->y -= r->y;
	return l;
}

inline vec2i* vec2i_scalar(vec2i* l, int16_t s){
	l->x *= s;
	l->y *= s;
	return l;
}

inline int16_t vec2i_dot(vec2i* l, vec2i* r){
	return l->x*r->x + l->y*r->y;
}

typedef struct
{
	int16_t x,y,z;
} vec3i;


#endif