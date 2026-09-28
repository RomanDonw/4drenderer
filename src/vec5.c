#include "vec5.h"

#include <math.h>

float vec5f_len(const vec5f in)
{ return sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2] + in[3] * in[3] + in[4] * in[4]); }

void vec5f_norm(vec5f out, const vec5f in)
{
    float l = vec5f_len(in);
    out[0] = in[0] / l;
    out[1] = in[1] / l;
    out[2] = in[2] / l;
    out[3] = in[3] / l;
    out[4] = in[4] / l;
}

void vec5f_norm2(vec5f inout)
{
    float l = vec5f_len(inout);
    inout[0] = inout[0] / l;
    inout[1] = inout[1] / l;
    inout[2] = inout[2] / l;
    inout[3] = inout[3] / l;
    inout[4] = inout[4] / l;
}