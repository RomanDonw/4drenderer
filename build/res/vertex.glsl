#version 330 core

layout (location = 0) in vec4 pos4;

uniform vec4 color;
uniform float model[25];
uniform mat4 perp;

void vec5f_fromvec4(out float outv[5], vec4 inv);
void mat5f_mulm(out float outv[25], const float a[25], const float b[25]);
void mat5f_mulv(out float outv[5], const float mat[25], const float vec[5]);

out vec4 wpos;

void main(void)
{
    float pos5[5];
    vec5f_fromvec4(pos5, pos4);

    float pos5trans[5];
    mat5f_mulv(pos5trans, model, pos5);

    wpos = vec4(pos5trans[0], pos5trans[1], pos5trans[2], pos5trans[3]);
    gl_Position = perp * vec4(pos5trans[0], pos5trans[1], pos5trans[2], 1);
}

#define MULMCOL(r, a1, a2, a3, a4, a5, b1, b2, b3, b4, b5) \
    outv[r] = a[a1] * b[b1] + a[a2] * b[b2] + a[a3] * b[b3] + a[a4] * b[b4] + a[a5] * b[b5];

#define MULMROW(a1, a2, a3, a4, a5) \
    MULMCOL(a1, a1, a2, a3, a4, a5, 0, 5, 10, 15, 20)\
    MULMCOL(a2, a1, a2, a3, a4, a5, 1, 6, 11, 16, 21)\
    MULMCOL(a3, a1, a2, a3, a4, a5, 2, 7, 12, 17, 22)\
    MULMCOL(a4, a1, a2, a3, a4, a5, 3, 8, 13, 18, 23)\
    MULMCOL(a5, a1, a2, a3, a4, a5, 4, 9, 14, 19, 24)

void mat5f_mulm(out float outv[25], const float a[25], const float b[25])
{
    MULMROW(0, 1, 2, 3, 4)
    MULMROW(5, 6, 7, 8, 9)
    MULMROW(10, 11, 12, 13, 14)
    MULMROW(15, 16, 17, 18, 19)
    MULMROW(20, 21, 22, 23, 24)
}

#define MULV(r, a1, a2, a3, a4, a5) \
    outv[r] = mat[a1] * vec[0] + mat[a2] * vec[1] + mat[a3] * vec[2] + mat[a4] * vec[3] + mat[a5] * vec[4];

void mat5f_mulv(out float outv[5], const float mat[25], const float vec[5])
{
    MULV(0, 0, 1, 2, 3, 4)
    MULV(1, 5, 6, 7, 8, 9)
    MULV(2, 10, 11, 12, 13, 14)
    MULV(3, 15, 16, 17, 18, 19)
    MULV(4, 20, 21, 22, 23, 24)
}

void vec5f_fromvec4(out float outv[5], vec4 inv)
{
    outv[0] = inv.x;
    outv[1] = inv.y;
    outv[2] = inv.z;
    outv[3] = inv.w;
    outv[4] = 1;
}