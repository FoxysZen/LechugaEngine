#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 uv;

void main()
{
    fragPos = vec3(view * model * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(view * model))) * normal;
    uv = texCoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}