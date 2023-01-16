#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
flat in int instanceId;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform bool picking;
uniform int pickedID;
//0: draw normal, 1: only draw picked asteroid with write to stencil buffer, 2: draw picked with asteroid with solid color
uniform int outlining;

const vec3 k_s = vec3(0.1f);
const vec3 k_a = vec3(0.09f, 0.77f, 0.97f);
const float ambientStrength = 0.2f;
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
    if ((outlining > 0 && pickedID != instanceId) || (outlining == 0 && pickedID == instanceId)) {
        discard;
    }

    if (picking) {
        fragColor = vec4(instanceIdToColor(), 1.0f);
    } else {
        if (outlining == 2) {
            fragColor = vec4(0.8, 0.0, 0.0, 1.0);
        } else {
            vec3 N = normalize(worldNormal);
            vec3 V = normalize(cameraPos - worldPosition);
            vec3 R = normalize(reflect((-1)*V, N));

            vec3 L = normalize(lightPos - worldPosition);

            vec3 k_d;
            if (instanceId != pickedID) {
                k_d = vec3(0.5);
            } else {

            }

            vec3 diffuse = k_d * max(0.0, dot(L, N));
            vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
            //ambient: https://learnopengl.com/Lighting/Basic-Lighting
            vec3 ambient = k_a * ambientStrength * k_d;
            vec3 sum = diffuse + specular + ambient;

            /* leave out for now, gets too bright when close to the sphere*/
            //     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
            //     vec3 intensity = light_intensity / distance;
            fragColor = vec4(sum /** intensity*/, 1.0);
        }
    }
}
