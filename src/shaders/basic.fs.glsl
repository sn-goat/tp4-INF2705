#version 330 core

in vec2 vTexCoord;

uniform sampler2D diffuseSampler;

out vec4 fragColor;

void main()
{
    fragColor = texture(diffuseSampler, vTexCoord);
}
