#ifndef MAT5_H
#define MAT5_H

#include "vec4.h"

typedef float mat5f[25];
typedef float vec5f[5];

void mat5f_zero(mat5f out);
void mat5f_idt(mat5f out);
void mat5f_copy(mat5f out, const mat5f in);

void mat5f_translate(mat5f out, const vec4f in);
void mat5f_scale(mat5f out, const vec4f in);

void mat5f_rotatexy(mat5f out, float angle);
void mat5f_rotateyz(mat5f out, float angle);
void mat5f_rotatezx(mat5f out, float angle);

void mat5f_rotatexw(mat5f out, float angle);
void mat5f_rotateyw(mat5f out, float angle);
void mat5f_rotatezw(mat5f out, float angle);

void mat5f_mulm(mat5f out, const mat5f a, const mat5f b);
void mat5f_mulm2(mat5f out, const mat5f in); // out = out * in
void mat5f_mulm2r(mat5f out, const mat5f in); // out = in * out
void mat5f_mulv(vec5f out, const mat5f mat, const vec5f vec);
void mat5f_mulv2(vec5f out, const mat5f mat);

#endif