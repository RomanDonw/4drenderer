/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef VEC5_H
#define VEC5_H

typedef float vec5f[5];

float vec5f_len(const vec5f in);
void vec5f_norm(vec5f out, const vec5f in);
void vec5f_norm2(vec5f inout);

#endif