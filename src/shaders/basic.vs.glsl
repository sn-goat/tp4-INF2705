#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 mvp;
uniform mat4 modelView;
uniform mat3 normalMatrix;

out vec3 vPosView;
out vec3 vNormalView;
out vec2 vTexCoord;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    vPosView = vec3(modelView * vec4(aPos, 1.0));
    vNormalView = normalize(normalMatrix * aNormal);
    vTexCoord = aTexCoord;
}
