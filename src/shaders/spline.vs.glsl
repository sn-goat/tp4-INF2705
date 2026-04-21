#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aT;

uniform mat4 mvp;
uniform float time;

out float vT;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    vT = aT;
}
