#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    worldNormal = vec3(model * vec4(normal, 0.0));
    worldPosition = vec3(model * vec4(position, 1.0));
    TexCoord = vec3(vec2(position),0);
}
