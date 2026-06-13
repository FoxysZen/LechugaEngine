#version 410 core

in vec2 uv;

uniform sampler2D skyTexture;
uniform float time;

out vec4 fragColor;
in vec3 fragPos;

void main()
{
    const float PI = 3.14159265358979323846;
    float cosT = cos(time);
    float sinT = sin(time);
    vec3 rotatedPos = vec3(
        fragPos.x * cosT - fragPos.z * sinT,
        fragPos.y,
        fragPos.x * sinT + fragPos.z * cosT
    );
    vec2 sphereUV;
    sphereUV.x = atan(rotatedPos.z, rotatedPos.x) / (2.0 * PI) + 0.5;
    sphereUV.y = acos(rotatedPos.y / length(rotatedPos)) / PI;
    fragColor = texture(skyTexture, sphereUV);
}