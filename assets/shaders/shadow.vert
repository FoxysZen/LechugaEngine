#version 410 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 boneWeights;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 64;
uniform mat4 boneMatrices[MAX_BONES];

void main()
{
    mat4 skinMatrix = boneMatrices[boneIDs.x] * boneWeights.x
                    + boneMatrices[boneIDs.y] * boneWeights.y
                    + boneMatrices[boneIDs.z] * boneWeights.z
                    + boneMatrices[boneIDs.w] * boneWeights.w;

    vec4 skinnedPos = skinMatrix * vec4(position, 1.0);

    gl_Position = lightSpaceMatrix * model * skinnedPos;
}