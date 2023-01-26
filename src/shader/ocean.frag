#version 460 core
out vec4 FragColor;

in vec3 worldNormal;
in vec3 worldPosition;
in vec2 TexCoord;
in vec3 Color;
void main() {
    
    FragColor = vec4(Color,1.0);

}