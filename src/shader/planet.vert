#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
out vec3 TexCoord;
out vec4 lightSpacePosition;
flat out int inCrater;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;
uniform bool depthRender;

void main()
{
    if (depthRender) {
        gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
    } else {
        inCrater = floatBitsToInt(position.x) & 1;

        float texture_U = asin(normal[0])/2 +0.5;
        float texture_V = asin(normal[1])/2 +0.5;
        gl_Position = projection * view * model * vec4(position, 1.0);
        worldNormal = vec3(model * vec4(normal, 0.0));
        worldPosition = vec3(model * vec4(position, 1.0));
        lightSpacePosition = lightSpaceMatrix * vec4(worldPosition, 1.0);
        TexCoord = position;
    }
}
