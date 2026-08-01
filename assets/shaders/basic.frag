#version 410 core
#define MAX_LIGHTS 16

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPos;

// Material settings
uniform bool cellShaded;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform int matShin;

uniform sampler2D textures[8];
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform int numLights;

out vec4 fragColor;

vec3 calcAmbient(vec3 color)
{
    return color;
}

vec3 calcDiffuse(vec3 color, vec3 norm, vec3 lightDir)
{
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * matDiffuse * color;
}

vec3 calcSpecular(vec3 color, vec3 norm, vec3 lightDir)
{
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShin);
    return 0.0 + matSpecular * spec * color;
}

vec3 specular (vec3 color, vec3 norm, vec3 lightDir)
{
    vec3 colRes = vec3(0);

    if ((dot(norm,lightDir) < 0) || (matShin == 0))
        return colRes;

    vec3 R = reflect(-lightDir, norm);
    vec3 V = normalize(-fragPos);

    if (dot(R, V) < 0)
        return colRes;

    float shine = pow(max(0.0, dot(R, V)), matShin);
    return (colRes + matSpecular * color * shine);
}

vec3 calcDiffuseCellShading(vec3 color, vec3 norm, vec3 lightDir)
{
    float diff = max(dot(norm, lightDir), 0.0);
    if (diff > 0.6) diff = 1.0;
    else if (diff > 0.3) diff = 0.5;
    else diff = 0.1;
    return diff * matDiffuse * color;
}

vec3 calcSpecularCellShading(vec3 color, vec3 norm, vec3 lightDir)
{
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShin);
    if (spec > 0.5) spec = 1.0;
    else spec = 0.0;
    return matSpecular * spec * color;
}

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 result = calcAmbient(lightColor[0]);

    for (int i = 0; i < numLights; ++i)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);
        vec3 diffuse = vec3(0.0);
        if (cellShaded)
        {
            diffuse = calcDiffuseCellShading(lightColor[i], norm, lightDir);
        }
        else
        {
            diffuse = calcDiffuse(lightColor[i], norm, lightDir);
        }

        result += diffuse;

        result += specular(lightColor[i], norm, lightDir);
    }

    vec4 texColor = texture(textures[0], uv);
    fragColor = vec4(result, 1.0) * texColor;
}

// Normal shader
//void main()
//{
//    fragColor = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0);
//}