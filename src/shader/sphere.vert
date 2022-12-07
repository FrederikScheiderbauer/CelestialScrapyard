#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
out vec3 TexCoord;
flat out int inCrater;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    inCrater = floatBitsToInt(position.x) & 1;

    float texture_U = asin(normal[0])/2 +0.5;
    float texture_V = asin(normal[1])/2 +0.5;
    gl_Position = projection * view * model * vec4(position, 1.0);
    worldNormal = vec3(model * vec4(normal, 0.0));
    worldPosition = vec3(model * vec4(position, 1.0));
    if(length(worldPosition) > 1.0f) {
        TexCoord = vec3(vec2(position),0);
    } else {
        TexCoord = vec3(vec2(position),1);
    }
    //TexCoord = vec3(vec2(position),0);
}
