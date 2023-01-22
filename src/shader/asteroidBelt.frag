#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec4 lightSpacePosition;
flat in int instanceId;
out vec4 fragColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform bool picking;
uniform int pickedID;
//0: draw normal, 1: only draw picked asteroid with write to stencil buffer, 2: draw picked with asteroid with solid color
uniform int outlining;
uniform bool depthRender;

layout(binding = 15) uniform sampler2D depthMap;

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

//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float bias = 0.005;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    if (depthRender) {
        //fragColor = vec4(1.0f);
        return;
    }
    if ((outlining > 0 && pickedID != instanceId) || (outlining == 0 && pickedID == instanceId)) {
        discard;
    }

    if (picking) {
        //gPosition is at out location 0 for deferred shading -> replaces fragColor for picking
        gPosition = instanceIdToColor();
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
            float shadow = ShadowCalculation(lightSpacePosition);
            vec3 sum = (1.0 - shadow) * (diffuse + specular) + ambient;

            // store the fragment position vector in the first gbuffer texture
            gPosition = worldPosition;
            // also store the per-fragment normals into the gbuffer
            gNormal = N;
            // and the diffuse per-fragment color
            gAlbedoSpec.rgb = k_d;
            gAlbedoSpec.a = shadow;

            /* leave out for now, gets too bright when close to the sphere*/
            //     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
            //     vec3 intensity = light_intensity / distance;
            //fragColor = vec4(sum /** intensity*/, 1.0);
            fragColor = vec4(1.0f);
        }
    }
}
