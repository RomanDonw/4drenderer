#ifndef MAT4_H
#define MAT4_H

typedef float mat4f[16];

void mat4f_zero(mat4f out);
void mat4f_idt(mat4f out);

void mat4f_perspective(mat4f out, float halfFOVy, float aspectratio, float near, float far);
void mat4f_print(const mat4f in);

#endif