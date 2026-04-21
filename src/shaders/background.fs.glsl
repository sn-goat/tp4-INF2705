#version 330 core

in vec2 vUV;
uniform float time;
out vec4 fragColor;

void main()
{
    // Vertical gradient: deep indigo at bottom -> violet-pink halfway -> near-black at top
    vec3 bottom = vec3(0.05, 0.02, 0.15);
    vec3 middle = vec3(0.35, 0.10, 0.45);
    vec3 top    = vec3(0.02, 0.02, 0.08);

    float t = vUV.y;
    vec3 col = mix(bottom, middle, smoothstep(0.0, 0.55, t));
    col = mix(col, top, smoothstep(0.55, 1.0, t));

    // Subtle vignette to push focus toward center
    vec2 c = vUV - 0.5;
    col *= 1.0 - dot(c, c) * 0.7;

    // Soft animated aura around centre for a "magic" feel
    float d = length(c);
    float aura = exp(-d * 6.0) * (0.15 + 0.1 * sin(time * 1.5));
    col += vec3(0.4, 0.3, 0.8) * aura;

    fragColor = vec4(col, 1.0);
}
