#version 410 core
#define MAX_LIGHTS 16

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 FragPosLightSpace;

uniform sampler2D textures[8];
uniform sampler2D shadowMap;
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

//float CalculateShadow(vec4 fragPosLightSpace, vec3 norm, vec3 sunDir)
//{
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    
//    // From [-1,1] to [0,1]
//    projCoords = projCoords * 0.5 + 0.5;
//    
//    if(projCoords.z > 1.0) return 0.0;
//
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    float currentDepth = projCoords.z;
//
//    float cosAngle = clamp(dot(norm, sunDir), 0.0, 1.0);
//    
//    if (cosAngle < 0.05) return 1.0;
//
//    float slopeScale = sqrt(1.0 - cosAngle * cosAngle) / max(cosAngle, 0.0001);
//    float bias = 0.0001 + 0.0005 * slopeScale;
//    bias = min(bias, 0.008);
//
//    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
//}

//float CalculateShadow(vec4 fragPosLightSpace, vec3 norm, vec3 sunDir)
//{
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    
//    // From [-1,1] to [0,1]
//    projCoords = projCoords * 0.5 + 0.5;
//    
//    if(projCoords.z > 1.0) return 0.0;
//
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    float currentDepth = projCoords.z;
//
//    float bias = 0.0005; 
//    
//    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
//}
//
//void main()
//{
//    vec3 norm = normalize(fragNormal);
//    vec3 result = vec3(0.0);
//
//    vec3 sunDir = normalize(lightPos[0] - fragPos);
//    float shadow = CalculateShadow(FragPosLightSpace, norm, sunDir);
//
//    for (int i = 0; i < numLights; i++)
//    {
//        vec3 lightDir = normalize(lightPos[i] - fragPos);
//
//        result += calcAmbient(lightColor[i]);
//
//        vec3 diffuse = calcDiffuseCellShading(lightColor[i], norm, lightDir);
//
//        if (i == 0)
//        {
//            result += (1.0 - shadow) * diffuse;
//        }
//        else
//        {
//            result += diffuse;
//        }
//    }
//
//    vec4 texColor = texture(textures[0], uv);
//    fragColor = vec4(result, 1.0) * texColor;
//}

float CalculateShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // From [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.0015; 
    
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 result = vec3(0.0);

    float shadow = CalculateShadow(FragPosLightSpace);

    for (int i = 0; i < numLights; i++)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);
        result += calcAmbient(lightColor[i]);
        vec3 diffuse = calcDiffuseCellShading(lightColor[i], norm, lightDir);

        if (i == 0)
        {
            result += (1.0 - shadow) * diffuse;
        }
        else
        {
            result += diffuse;
        }
    }

    vec4 texColor = texture(textures[0], uv);
    fragColor = vec4(result, 1.0) * texColor;
}

// Normal shader
//void main()
//{
//    fragColor = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0);
//}