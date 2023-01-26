#version 460 core
out vec4 fragColor;

layout(binding = 0, rgba16f) uniform image2D gPosition;
layout(binding = 1, rgba16f) uniform image2D gNormal;
layout(binding = 2, rgba16f) uniform image2D gAlbedoSpec;
layout(binding = 3, r16f) uniform image2D ssao;
layout(binding = 15) uniform sampler2D depthMap;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform mat4 lightSpaceMatrix;
uniform bool useSSAO;

const vec3 k_a = vec3(0.09f, 0.77f, 0.97f);
const float ambientStrength = 0.2f;
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

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
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec3 worldPosition = imageLoad(gPosition, ivec2(fragCoord)).rgb;
    vec4 lightSpacePosition = lightSpaceMatrix * vec4(worldPosition, 1.0);
    vec3 N = imageLoad(gNormal, ivec2(fragCoord)).rgb;
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));
    vec3 L = normalize(lightPos - worldPosition);
    vec4 albedoSpec = imageLoad(gAlbedoSpec, ivec2(fragCoord)).rgba;
    vec3 k_d = albedoSpec.rgb;
    vec3 k_s = vec3(0.1f);
    //check for trees
    if (albedoSpec.a != 0.f) {
        // 1 / 0.8 * 0.2 = 0.25
        k_s = 0.25f * k_d;
    }

    float shadow = ShadowCalculation(lightSpacePosition);
    float ambientOcclusion = useSSAO ? imageLoad(ssao, ivec2(fragCoord)).r : 1.f;

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    //ambient: https://learnopengl.com/Lighting/Basic-Lighting
    vec3 ambient = ambientOcclusion * k_a * ambientStrength * k_d;
    vec3 sum = (1.0 - shadow) * (diffuse + specular) + ambient;

    fragColor = vec4(sum , 1.0);
}
