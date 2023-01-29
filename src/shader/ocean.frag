#version 460 core
out vec4 FragColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 worldNormal;
in vec3 worldPosition;
in vec2 TexCoord;
in vec3 Color;
void main() {
    
    gPosition = worldPosition;
    gNormal = normalize(worldNormal);
    gAlbedoSpec.rgb = Color;
    gAlbedoSpec.a = 0.f;
    //FragColor = vec4(Color,1.0);

}