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

vec3 calcDiffuse(vec3 color, vec3 norm, vec3 lightDir)
{
    float diff = max(dot(norm, lightDir), 0.0);
    if (cellShaded)
    {
        if (diff > 0.6) diff = 1.0;
        else if (diff > 0.3) diff = 0.5;
        else diff = 0.1;
    }
    return diff * matDiffuse * color;
}


vec3 calcSpecular(vec3 lightColor, vec3 norm, vec3 lightDir, vec3 viewDir)
{
    if (dot(norm, lightDir) <= 0.0 || matShin <= 0)
    {
        return vec3(0.0);
    }

    vec3 reflectDir = reflect(-lightDir, norm);
    float specFactor = max(dot(viewDir, reflectDir), 0.0);
    float specIntensity = pow(specFactor, float(matShin));

    if (cellShaded)
    {
        specIntensity = step(0.5, specIntensity);
    }

    return matSpecular * lightColor * specIntensity;
}

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPos);

    vec3 ambient = vec3(0.5) * matDiffuse;
    vec3 result = ambient;

    for (int i = 0; i < numLights; ++i)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);

        vec3 diffuse = calcDiffuse(lightColor[i], norm, lightDir);
        vec3 specular = calcSpecular(lightColor[i], norm, lightDir, viewDir);

        result += diffuse + specular;
    }

    vec4 texColor = texture(textures[0], uv);

    if (texColor.a < 0.5) 
    {
        discard;
    }

    fragColor = vec4(result, 1.0) * texColor;
}

// Normal shader
//void main()
//{
//    fragColor = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0);
//}