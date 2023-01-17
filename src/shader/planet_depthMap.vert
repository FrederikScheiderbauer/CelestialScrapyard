#version 460
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
}
