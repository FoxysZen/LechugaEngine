#version 410 core
uniform vec3 color;

in vec4 fragColor_v;
in vec2 uv;

uniform bool useTexture;
uniform sampler2D particleTexture;

out vec4 fragColor;

void main()
{
    if (useTexture)
    {
        fragColor = texture(particleTexture, uv);
    }
    else
    {
        fragColor = fragColor_v;
    }
}
