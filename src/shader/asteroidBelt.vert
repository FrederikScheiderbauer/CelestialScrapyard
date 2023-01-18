#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
flat out int instanceId;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform int pickedID;
uniform int outlining;
uniform bool depthRender;

layout(std430, binding = 0) buffer offsetBuffer {
    vec4 offsets[];
};

#define PI 3.1415926538

//based on: https://www.shadertoy.com/view/4djSRW
float hash11(inout float p)
{
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

//https://gist.github.com/neilmendoza/4512992#file-gistfile1-txt
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,  0.0,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c, 0.0,
    0.0, 0.0, 0.0, 1.0);
}

void main()
{
    if (outlining > 0 && pickedID != gl_InstanceID) {
        worldPosition = vec3(0.0);
        gl_Position = vec4(0.0);
        worldNormal = vec3(0.0);
        instanceId = gl_InstanceID;
    } else {
        float seed = float(gl_InstanceID);
        vec3 axis = normalize(vec3(hash11(seed), hash11(seed), hash11(seed)));
        float angle = 2 * PI * hash11(seed);
        mat4 rotation = rotationMatrix(axis, angle);

        worldPosition = vec3(rotation * model * vec4(position, 1.0) + offsets[gl_InstanceID]);
        gl_Position = projection * view * vec4(worldPosition, 1.0);
        if (!depthRender) {
            worldNormal = vec3(rotation * model * vec4(normal, 0.0));
            instanceId = gl_InstanceID;
        }
    }
}
