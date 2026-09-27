#include "mat5.h"

#include <string.h>
#include <math.h>

void mat5f_zero(mat5f out) { memset(out, 0, sizeof(mat5f)); }

void mat5f_idt(mat5f out)
{
    mat5f_zero(out);
    out[0] = out[6] = out[12] = out[18] = out[24] = 1;
}

void mat5f_copy(mat5f out, const mat5f in) { memcpy(out, in, sizeof(mat5f)); }

void mat5f_translate(mat5f out, const vec4f in)
{
    mat5f_idt(out);
    out[4] = in[0];
    out[9] = in[1];
    out[14] = in[2];
    out[19] = in[3];
}

void mat5f_scale(mat5f out, const vec4f in)
{
    mat5f_zero(out);
    out[0] = in[0];
    out[6] = in[1];
    out[12] = in[2];
    out[18] = in[3];
    out[24] = 1;
}

#define MULMCOL(r, a1, a2, a3, a4, a5, b1, b2, b3, b4, b5) \
    out[r] = a[a1] * b[b1] + a[a2] * b[b2] + a[a3] * b[b3] + a[a4] * b[b4] + a[a5] * b[b5];

#define MULMROW(a1, a2, a3, a4, a5) \
    MULMCOL(a1, a1, a2, a3, a4, a5, 0, 5, 10, 15, 20)\
    MULMCOL(a2, a1, a2, a3, a4, a5, 1, 6, 11, 16, 21)\
    MULMCOL(a3, a1, a2, a3, a4, a5, 2, 7, 12, 17, 22)\
    MULMCOL(a4, a1, a2, a3, a4, a5, 3, 8, 13, 18, 23)\
    MULMCOL(a5, a1, a2, a3, a4, a5, 4, 9, 14, 19, 24)

void mat5f_mulm(mat5f out, const mat5f a, const mat5f b)
{
    MULMROW(0, 1, 2, 3, 4)
    MULMROW(5, 6, 7, 8, 9)
    MULMROW(10, 11, 12, 13, 14)
    MULMROW(15, 16, 17, 18, 19)
    MULMROW(20, 21, 22, 23, 24)
}

#define MULV(r, a1, a2, a3, a4, a5) \
    out[r] = mat[a1] * vec[0] + mat[a2] * vec[1] + mat[a3] * vec[2] + mat[a4] * vec[3] + mat[a5] * vec[4];

void mat5f_mulv(vec5f out, const mat5f mat, const vec5f vec)
{
    MULV(0, 0, 1, 2, 3, 4)
    MULV(1, 5, 6, 7, 8, 9)
    MULV(2, 10, 11, 12, 13, 14)
    MULV(3, 15, 16, 17, 18, 19)
    MULV(4, 20, 21, 22, 23, 24)
}

void mat5f_mulm2(mat5f out, const mat5f in)
{
    mat5f ret;
    mat5f_mulm(ret, out, in);
    mat5f_copy(out, ret);
}

void mat5f_mulm2r(mat5f out, const mat5f in)
{
    mat5f ret;
    mat5f_mulm(ret, in, out);
    mat5f_copy(out, ret);
}

#define ROTFUNC(functionname, cos1idx, cos2idx, possinidx, negsinidx, first1idx, second1idx) \
    void functionname(mat5f out, float angle) \
    {\
        mat5f_zero(out);\
        out[cos1idx] = out[cos2idx] = cosf(angle);\
        out[negsinidx] = -(out[possinidx] = sinf(angle));\
        out[first1idx] = out[second1idx] = out[24] = 1;\
    }

ROTFUNC(mat5f_rotatexy, 0, 6, 5, 1, 12, 18)
ROTFUNC(mat5f_rotateyz, 6, 12, 11, 7, 0, 18)
ROTFUNC(mat5f_rotatezx, 0, 12, 2, 10, 6, 18)

ROTFUNC(mat5f_rotatexw, 0, 18, 15, 3, 6, 12)
ROTFUNC(mat5f_rotateyw, 6, 18, 16, 8, 0, 12)
ROTFUNC(mat5f_rotatezw, 12, 18, 17, 13, 0, 6)

void mat5f_lookat(mat5f out, const vec4f pos, const vec4f front, const vec4f right, const vec4f up, const vec4f over)
{
    out[20] = out[21] = out[22] = out[23] = 0;
    out[24] = 1;
    vec4f_copy((void *)out, right);
    vec4f_copy((void *)(&out[5]), up);
    vec4f_copy((void *)(&out[10]), front);
    vec4f_copy((void *)(&out[15]), over);
    out[4] = -vec4f_dot(pos, right);
    out[9] = -vec4f_dot(pos, up);
    out[14] = -vec4f_dot(pos, front);
    out[19] = -vec4f_dot(pos, over);
}