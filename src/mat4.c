/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "mat4.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

void mat4f_zero(mat4f out) { memset(out, 0, sizeof(mat4f)); }
void mat4f_idt(mat4f out)
{
    mat4f_zero(out);
    out[0] = out[5] = out[10] = out[15] = 1;
}

void mat4f_perspectiveGL(mat4f out, float halfFOVy, float aspectratio, float near, float far)
{
    mat4f_zero(out);
    
    float tanfov = tanf(halfFOVy);
    float delta = far - near;
    out[0] = 1 / (aspectratio * tanfov);
    out[5] = 1 / tanfov;
    out[10] = -(far + near) / delta;
    out[14] = -(2 * far * near) / delta;
    out[11] = -1;
}

void mat4f_print(const mat4f in)
{ for (unsigned char i = 0; i < 4; i++) printf("%10f%10f%10f%10f\n", in[i * 4], in[i * 4 + 1], in[i * 4 + 2], in[i * 4 + 3]); }