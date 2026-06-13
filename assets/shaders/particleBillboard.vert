#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 instanceData;
layout(location = 2) in vec4 instanceColor;
layout(location = 3) in vec2 texCoord;

uniform mat4 view;
uniform mat4 proj;

out vec4 fragColor_v;
out vec2 uv;

void main()
{
    fragColor_v = instanceColor;
    uv = texCoord;

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);
    
    vec3 worldPos = instanceData.xyz 
              + (camRight * position.x * instanceData.w)
              + (camUp * position.y * instanceData.w);
              
    gl_Position = proj * view * vec4(worldPos, 1.0);
}