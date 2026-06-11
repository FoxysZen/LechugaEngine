#version 410 core
in vec2 uv;
uniform sampler2D skyTexture;
out vec4 fragColor;

void main()
{
    fragColor = texture(skyTexture, uv);
}