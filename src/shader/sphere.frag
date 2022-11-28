#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec2 TexCoord;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform sampler2D texture1;

const vec3 k_d = vec3(0.455, 0.478, 0.259);
const vec3 k_s = vec3(0.1f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));

    vec3 L = normalize(cameraPos - worldPosition);

    //vec3 k_d = N;
    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    vec3 sum = diffuse + specular;

/* leave out for now, gets too bright when close to the sphere*/
//     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
//     vec3 intensity = light_intensity / distance;

    fragColor = texture(texture1,TexCoord);//vec4(sum /** intensity*/, 1.0);
}
