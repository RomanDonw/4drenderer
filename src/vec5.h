#ifndef VEC5_H
#define VEC5_H

typedef float vec5f[5];

float vec5f_len(const vec5f in);
void vec5f_norm(vec5f out, const vec5f in);
void vec5f_norm2(vec5f inout);

#endif