#version 330 core

in float vT;
uniform float time;
out vec4 fragColor;

void main()
{
    // Pulse that travels along the curve
    float pulse = 0.5 + 0.5 * sin((vT - time * 0.6) * 12.566);
    vec3 cool = vec3(0.3, 0.6, 1.0);
    vec3 warm = vec3(1.0, 0.6, 0.9);
    vec3 col = mix(cool, warm, pulse);
    fragColor = vec4(col, 1.0);
}
