/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef MAT4_H
#define MAT4_H

typedef float mat4f[16];

void mat4f_zero(mat4f out);
void mat4f_idt(mat4f out);

// generates already transposed matrix (in column-major order) for OpenGL.
void mat4f_perspectiveGL(mat4f out, float halfFOVy, float aspectratio, float near, float far);
void mat4f_print(const mat4f in);

#endif