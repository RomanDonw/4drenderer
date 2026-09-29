/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef VEC4_H
#define VEC4_H

typedef float vec4f[4];

void vec4f_copy(vec4f out, const vec4f in);
float vec4f_dot(const vec4f a, const vec4f b);

float vec4f_len(const vec4f in);
void vec4f_norm(vec4f out, const vec4f in);
void vec4f_norm2(vec4f inout);

void vec4f_add(vec4f out, const vec4f a, const vec4f b);
void vec4f_add2(vec4f out, const vec4f in);
void vec4f_sub(vec4f out, const vec4f a, const vec4f b);
void vec4f_sub2(vec4f out, const vec4f in);
void vec4f_muls(vec4f out, const vec4f in, float scalar);
void vec4f_muls2(vec4f inout, float scalar);

#endif