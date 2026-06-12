#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 instanceData;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec3 camRight = normalize(vec3(view[0][0], view[1][0], view[2][0]));
    vec3 camUp    = normalize(vec3(view[0][1], view[1][1], view[2][1]));
    
    vec3 worldPos = instanceData.xyz 
              + camRight * position.x * instanceData.w
              + camUp    * position.y * instanceData.w;
              
    gl_Position = proj * view * vec4(worldPos, 1.0);
}