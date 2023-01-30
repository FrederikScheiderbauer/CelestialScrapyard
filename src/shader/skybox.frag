#version 460 core
layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = vec4(texture(skybox, TexCoords).rgb, 0.5);
}