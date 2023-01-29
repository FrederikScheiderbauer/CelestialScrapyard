#version 460 core
out vec4 fragColor;

in vec3 worldPosition;
layout(binding = 0, rgba16f) uniform image2D gPosition;
layout(binding = 1, rgba16f) uniform image2D gNormal;
layout(binding = 2, rgba16f) uniform image2D gAlbedoSpec;
layout(binding = 3, rgba16f) out vec4 refraction;
layout(binding = 0, rgba16f) uniform image2D oceanPosition;
layout(binding = 0, rgba16f) uniform image2D oceanNormal;

uniform vec3 cameraPos;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec4 position = imageLoad(oceanPosition,ivec2(fragCoord));
    vec4 normal = imageLoad(oceanNormal,ivec2(fragCoord));

    //calculate ray/vektor from camera to vertex/fragment
    vec3 rayToOceanSurface = position.xyz -cameraPos;
    //calculate refracted vektor
    float eta= 0.5;
    vec3 refractionVector = refract(rayToOceanSurface,normal.xyz);
    //determine color at that point;
    vec2 resultFragCoords;
    //set value of that coordinate in refraction texture to that value;
}