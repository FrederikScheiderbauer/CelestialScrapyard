#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Color;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

flat in vec3 worldNormal;
flat in vec3 worldPosition;

uniform sampler2D leaf_Texture;
uniform sampler2D Trunk_Texture;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform bool depthRender;

//const vec3 k_s = vec3(1.0f);
//const vec3 k_a = vec3(0.01f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

#define TREE_K_S_FLAG 1.f

//TODO: remove
//vec3 calculate_camera_light_contribution() {
//    vec3 k_d =0.8f* Color.rgb;
//    vec3 k_s = 0.2f * Color.rgb;
//    vec3 k_a = Color.rgb;
//    vec3 N = normalize(worldNormal);
//    vec3 V = normalize(cameraPos - worldPosition);
//    vec3 R = normalize(reflect((-1)*V, N));
//
//    vec3 L = normalize(lightPos - worldPosition);
//
//    vec3 diffuse = k_d * max(0.0, dot(L, N));
//    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
//
//    vec3 sum = diffuse + specular;
//    return sum;
//}

void main()
{
    if (depthRender) {
        return;
    }
/*
    vec3 k_d =0.8f* Color.rgb;
    vec3 k_s = 0.2f * Color.rgb;
    vec3 k_a = Color.rgb;
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));

    vec3 L = normalize(cameraPos - worldPosition);

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);

    vec3 sum = diffuse + specular;
    */

    vec3 k_d = 0.8f * Color.rgb;

    gPosition = worldPosition;
    gNormal = normalize(worldNormal);
    gAlbedoSpec.rgb = k_d;
    //set specular to one to indicate usage of tree illumination model
    gAlbedoSpec.a = TREE_K_S_FLAG;

    //FragColor = vec4(calculate_camera_light_contribution(), 1.0);
}