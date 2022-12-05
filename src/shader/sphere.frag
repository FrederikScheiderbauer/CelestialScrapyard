#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec3 TexCoord;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform sampler2D grassland;
uniform sampler2D snow;
uniform sampler2D mountain;
uniform sampler2D water;
uniform sampler2DArray textureArray;

const vec3 k_s = vec3(0.1f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));

    vec3 L = normalize(cameraPos - worldPosition);

    vec3 k_d;
    //k_d = texture(textureArray,TexCoord,0).rgb;
    if(length(worldPosition) > 1.5f) {
        k_d = texture(grassland,vec2(TexCoord)).rgb;
    } else {
        k_d = texture(water,vec2(TexCoord)).rgb;
    }
    if(length(worldPosition) > 1.7f) {
        k_d = texture(mountain,vec2(TexCoord)).rgb;
    }
        if(length(worldPosition) > 1.78f) {
        k_d = texture(snow,vec2(TexCoord)).rgb;
    }

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    vec3 sum = diffuse + specular;

/* leave out for now, gets too bright when close to the sphere*/
//     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
//     vec3 intensity = light_intensity / distance;
    fragColor = vec4(sum /** intensity*/, 1.0);
}
