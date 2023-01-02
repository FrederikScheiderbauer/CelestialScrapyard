#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
flat in int instanceId;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform bool picking;

const vec3 k_s = vec3(0.1f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

vec3 instanceIdToColor() {
    //http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
    float r = float((instanceId & 0x000000FF) >> 0) / 255.f;
    float g = float((instanceId & 0x0000FF00) >> 8) / 255.f;
    float b = float((instanceId & 0x00FF0000) >> 16) / 255.f;
    return vec3(r,g,b);
}

void main()
{
    if (picking) {
        fragColor = vec4(instanceIdToColor(), 1.0f);
    } else {
        vec3 N = normalize(worldNormal);
        vec3 V = normalize(cameraPos - worldPosition);
        vec3 R = normalize(reflect((-1)*V, N));

        vec3 L = normalize(cameraPos - worldPosition);

        vec3 k_d = vec3(0.5);

        vec3 diffuse = k_d * max(0.0, dot(L, N));
        vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
        vec3 sum = diffuse + specular;

        /* leave out for now, gets too bright when close to the sphere*/
        //     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
        //     vec3 intensity = light_intensity / distance;
        fragColor = vec4(sum /** intensity*/, 1.0);
    }
}
