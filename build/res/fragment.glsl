/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#version 330 core

in vec4 wpos;
in vec4 vpos;
out vec4 FragColor;

//uniform float camw;

void main(void)
{
    //if (vpos.w < -0.05 || vpos.w > 0.05) discard;
    FragColor = vec4(1, 1, 1, 1);
}