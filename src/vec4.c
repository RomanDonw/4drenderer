#include "vec4.h"

#include <string.h>

void vec4f_copy(vec4f out, const vec4f in) { memcpy(out, in, sizeof(vec4f)); }
float vec4f_dot(const vec4f a, const vec4f b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]; }