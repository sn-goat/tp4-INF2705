#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 mvp;

out vec2 vTexCoord;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
}
