#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
flat out int instanceId;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

layout(std430, binding = 0) buffer offsetBuffer {
    vec4 offsets[];
};

void main()
{
    worldPosition = vec3(model * vec4(position, 1.0) + offsets[gl_InstanceID]);
    gl_Position = projection * view * vec4(worldPosition, 1.0);
    worldNormal = vec3(model * vec4(normal, 0.0));
    instanceId = gl_InstanceID;
}
