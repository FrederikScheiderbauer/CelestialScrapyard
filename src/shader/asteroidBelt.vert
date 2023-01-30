#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 worldNormal;
out vec3 worldPosition;
out vec3 lightSourceColor;
flat out int instanceId;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;
uniform int pickedID;
uniform float scaleFactor;
//0 = standard draw, 1 = write to stencil buffer, 2 = render outline
uniform int outlining;
uniform bool depthRender;

layout(std430, binding = 0) buffer offsetBuffer {
    vec4 offsets[];
};

layout(std430, binding = 1) buffer scaleFactorBuffer {
    vec4 scaleFactors[];
};

#define PI 3.1415926538
const mat4 lightModel = mat4(2.0);

//based on: https://www.shadertoy.com/view/4djSRW
float hash11(inout float p)
{
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

//https://www.shadertoy.com/view/4djSRW
vec2 hash23(vec3 p3)
{
    p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
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

mat4 generateScaleMatrix(vec3 scale) {
    mat4 scale_Matrix;
    for ( int x = 0; x < 4; x++ ) {
        for ( int y = 0; y < 4; y++ ) {
            if ( x == y && x < 3 ) {
                scale_Matrix[x][y] = scale[y];
            }  
            else if (x == y) {
                scale_Matrix[x][y] = 1.0;
            } else {
                scale_Matrix[x][y] = 0.0;
            }
        }
    }
    return scale_Matrix;
}

void main()
{
    vec3 scaling_factor = vec3(length(offsets[gl_InstanceID])) - vec3(2.0);
    mat4 scalingMatrix = generateScaleMatrix(vec3(scaleFactors[gl_InstanceID]));
    //mat4 scalingMatrix = generateScaleMatrix(scaling_factor);

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

        if (gl_InstanceID == 0) {
            worldPosition = vec3(lightModel * vec4(normalize(position), 1.0) + 3.f * offsets[gl_InstanceID]);
            vec2 rand = hash23(position);
            if (rand.x < 0.5) {
                lightSourceColor = mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), rand.y);
            } else {
                lightSourceColor = mix(vec3(1.0), vec3(1.0, 1.0, 0.0), rand.y);
            }
        } else {
            //worldPosition = vec3( rotation  *  model * vec4(position, 1.0) + offsets[gl_InstanceID]);
            worldPosition = vec3( rotation  * scalingMatrix* model * vec4(position, 1.0) + offsets[gl_InstanceID]);
        }

        if (depthRender) {
            gl_Position = lightSpaceMatrix * vec4(worldPosition, 1.0);
        } else {
            gl_Position = projection * view * vec4(worldPosition, 1.0);
            if (gl_InstanceID == 0) {
                worldNormal = vec3(rotation * lightModel * vec4(normalize(position), 0.0));
            } else {
                worldNormal = vec3(rotation *  model * vec4(normal, 0.0));
            }
            instanceId = gl_InstanceID;
        }
    }
}
