#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 boneWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

const int MAX_BONES = 64;
uniform mat4 boneMatrices[MAX_BONES];

out vec3 fragPos;
out vec3 fragNormal;
out vec2 uv;

void main()
{
    mat4 skinMatrix = boneMatrices[boneIDs.x] * boneWeights.x
                    + boneMatrices[boneIDs.y] * boneWeights.y
                    + boneMatrices[boneIDs.z] * boneWeights.z
                    + boneMatrices[boneIDs.w] * boneWeights.w;

    vec4 skinnedPos = skinMatrix * vec4(position, 1.0);
    vec4 skinnedNormal = skinMatrix * vec4(normal, 0.0);

    fragPos = vec3(view * model * skinnedPos);
    fragNormal = mat3(transpose(inverse(view * model))) * vec3(skinnedNormal);
    uv = texCoord;

    gl_Position = proj * view * model * skinnedPos;
}