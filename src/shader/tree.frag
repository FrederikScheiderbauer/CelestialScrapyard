#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Color;

uniform sampler2D leaf_Texture;
uniform sampler2D Trunk_Texture;

void main()
{
    FragColor = vec4(Color.rgb, 1.0f);
}