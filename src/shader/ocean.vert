#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 worldNormal;
out vec3 worldPosition;
out vec2 TexCoord;
out vec3 Color;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 planet_info;

float water_level = planet_info[0] -0.05;

void main() {


    worldPosition = vec3(model * vec4(position*water_level,1.0));
    worldNormal = vec3(model * vec4(normal,1.0));
    TexCoord = vec2(0.0);
    Color = vec3(0.0,0.5,1.0);
    gl_Position = projection *view * model *vec4(position*water_level,1.0);

}