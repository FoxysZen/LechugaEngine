#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 view;
uniform mat4 proj;
uniform float time;

out vec2 uv;
out vec3 fragPos;

void main()
{
    fragPos = position;
    uv = texCoord + vec2(time, 0.0);
    mat4 viewNoTranslation = mat4(mat3(view));
    gl_Position = proj * viewNoTranslation * vec4(position, 1.0);
}