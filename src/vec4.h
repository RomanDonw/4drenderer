#ifndef VEC4_H
#define VEC4_H

typedef float vec4f[4];

void vec4f_copy(vec4f out, const vec4f in);
float vec4f_dot(const vec4f a, const vec4f b);

#endif