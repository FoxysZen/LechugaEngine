#version 410 core

in vec2 uv;
uniform sampler2D texture0;
uniform sampler2D texture1;
out vec4 fragColor;

void main()
{
    fragColor = texture(texture0, uv);
}