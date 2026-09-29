#ifndef VEC4_H
#define VEC4_H

typedef float vec4f[4];

void vec4f_copy(vec4f out, const vec4f in);
float vec4f_dot(const vec4f a, const vec4f b);

float vec4f_len(const vec4f in);
void vec4f_norm(vec4f out, const vec4f in);
void vec4f_norm2(vec4f inout);

#endif