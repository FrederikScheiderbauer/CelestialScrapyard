#version 460 core
out vec4 fragColor;

layout(binding = 0, rgba16f) uniform image2D gPosition;
layout(binding = 1, rgba16f) uniform image2D gNormal;
layout(binding = 2, rgba16f) uniform image2D gAlbedoSpec;

uniform vec3 cameraPos;
uniform vec3 lightPos;

const vec3 k_s = vec3(0.1f);
const vec3 k_a = vec3(0.09f, 0.77f, 0.97f);
const float ambientStrength = 0.2f;
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec3 worldPosition = imageLoad(gPosition, ivec2(fragCoord)).rgb;
    vec3 N = imageLoad(gNormal, ivec2(fragCoord)).rgb;
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));
    vec3 L = normalize(lightPos - worldPosition);

    vec3 k_d = imageLoad(gAlbedoSpec, ivec2(fragCoord)).rgb;
    float shadow = imageLoad(gAlbedoSpec, ivec2(fragCoord)).a;

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    //ambient: https://learnopengl.com/Lighting/Basic-Lighting
    vec3 ambient = k_a * ambientStrength * k_d;
    vec3 sum = (1.0 - shadow) * (diffuse + specular) + ambient;

    fragColor = vec4(sum , 1.0);
}
