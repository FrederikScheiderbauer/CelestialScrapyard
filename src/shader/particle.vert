#version 460

layout (location = 0) in vec3 a_Position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

flat out vec4 u_Color;

struct ParticleInfo {
    //xyz: position, w: size
    vec4 positionAndSize;
    vec4 color;
};
layout(std430, binding = 0) buffer particleInfoBuffer {
    ParticleInfo particleInfos[];
};

void main()
{
    //Billboarding: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
    vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPosition_worldspace = particleInfos[gl_InstanceID].positionAndSize.xyz
        + CameraRight_worldspace * a_Position.x * particleInfos[gl_InstanceID].positionAndSize.w
        + CameraUp_worldspace * a_Position.y * particleInfos[gl_InstanceID].positionAndSize.w;

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);
    u_Color = particleInfos[gl_InstanceID].color;
}
