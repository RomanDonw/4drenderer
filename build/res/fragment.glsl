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