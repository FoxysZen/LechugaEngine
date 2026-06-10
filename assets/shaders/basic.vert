#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 uv;

void main()
{
    uv = texCoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}