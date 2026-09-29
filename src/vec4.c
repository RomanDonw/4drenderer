/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "vec4.h"

#include <string.h>
#include <math.h>

void vec4f_copy(vec4f out, const vec4f in) { memcpy(out, in, sizeof(vec4f)); }
float vec4f_dot(const vec4f a, const vec4f b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]; }

float vec4f_len(const vec4f in)
{ return sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2] + in[3] * in[3]); }

void vec4f_norm(vec4f out, const vec4f in)
{
    float l = vec4f_len(in);
    out[0] = in[0] / l;
    out[1] = in[1] / l;
    out[2] = in[2] / l;
    out[3] = in[3] / l;
}

void vec4f_norm2(vec4f inout)
{
    float l = vec4f_len(inout);
    inout[0] = inout[0] / l;
    inout[1] = inout[1] / l;
    inout[2] = inout[2] / l;
    inout[3] = inout[3] / l;
}