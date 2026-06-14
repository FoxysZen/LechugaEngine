#version 410 core
#define MAX_LIGHTS 16

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D textures[8];
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform int numLights;

out vec4 fragColor;

vec3 calcAmbient(vec3 color)
{
    return 0.1 * color;
}

vec3 calcDiffuse(vec3 color, vec3 norm, vec3 lightDir)
{
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * color;
}

vec3 calcSpecular(vec3 color, vec3 norm, vec3 lightDir)
{
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    return 0.5 * spec * color;
}

vec3 calcDiffuseCellShading(vec3 color, vec3 norm, vec3 lightDir)
{
    float diff = max(dot(norm, lightDir), 0.0);
    if (diff > 0.6)      diff = 1.0;
    else if (diff > 0.3) diff = 0.5;
    else                 diff = 0.1;
    return diff * color;
}

vec3 calcSpecularCellShading(vec3 color, vec3 norm, vec3 lightDir)
{
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    if (spec > 0.5) spec = 1.0;
    else            spec = 0.0;
    return 0.5 * spec * color;
}

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; i++)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);
        result += calcAmbient(lightColor[i]);
        result += calcDiffuseCellShading(lightColor[i], norm, lightDir);
        result += calcSpecularCellShading(lightColor[i], norm, lightDir);
    }

    vec4 texColor = texture(textures[0], uv);
    fragColor = vec4(result, 1.0) * texColor;
}