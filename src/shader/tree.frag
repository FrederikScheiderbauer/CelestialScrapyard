#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Color;

in vec3 worldNormal;
in vec3 worldPosition;

uniform sampler2D leaf_Texture;
uniform sampler2D Trunk_Texture;

uniform vec3 cameraPos;


//const vec3 k_s = vec3(1.0f);
//const vec3 k_a = vec3(0.01f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

vec3 calculate_camera_light_contribution() {
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
    return sum;
}
void main()
{
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
    FragColor = vec4(calculate_camera_light_contribution(), 1.0);
}