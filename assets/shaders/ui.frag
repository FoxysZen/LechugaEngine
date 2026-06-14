#version 410 core

in vec2 uv;

uniform vec3 color;
uniform bool useTexture;
uniform sampler2D uiTexture;

out vec4 fragColor;

void main()
{
    if (useTexture)
    {
        fragColor = texture(uiTexture, uv);
    }
    else
    {
        fragColor = vec4(color, 1.0);
    }
}