#version 330 core

in vec4 wpos;
out vec4 FragColor;

uniform float camw;

void main(void)
{
    if (wpos.w < camw - 0.05 || wpos.w > camw + 0.05) discard;
    FragColor = vec4(1, 1, 1, 1);
}