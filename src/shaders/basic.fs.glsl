#version 330 core

in vec3 vPosView;
in vec3 vNormalView;
in vec2 vTexCoord;

uniform sampler2D diffuseSampler;

uniform vec3 ambientColor;
uniform vec3 dirLightDirView;
uniform vec3 dirLightColor;
uniform vec3 pointLightPosView;
uniform vec3 pointLightColor;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;

out vec4 fragColor;

vec3 blinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor)
{
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3  halfway = normalize(lightDir + viewDir);
    float specularFactor = max(dot(normal, halfway), 0.0);

    vec3 diffuse = kd * diffuseFactor * lightColor;
    vec3 specular = ks * pow(specularFactor, shininess) * lightColor;
    return diffuse + specular;
}

void main()
{
    vec3 texColor = texture(diffuseSampler, vTexCoord).rgb;

    vec3 normal = normalize(vNormalView);
    vec3 viewDir = normalize(-vPosView);

    vec3 color = ka * ambientColor;

    vec3 dirLightDir = normalize(-dirLightDirView);
    color += blinnPhong(normal, dirLightDir, viewDir, dirLightColor);

    vec3  toLight = pointLightPosView - vPosView;
    float dist = length(toLight);
    vec3  pointLightDir = toLight / dist;
    float attenuation = 1.0 / (dist * dist);
    color += attenuation * blinnPhong(normal, pointLightDir, viewDir, pointLightColor);

    fragColor = vec4(color * texColor, 1.0);
}
