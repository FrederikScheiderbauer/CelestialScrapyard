#version 460 core
out vec4 fragColor;

layout(binding = 0, rgba16f) uniform image2D gPosition;
layout(binding = 1, rgba16f) uniform image2D gNormal;
layout(binding = 2, rgba16f) uniform image2D gAlbedoSpec;
layout(binding = 3, r16f) uniform image2D ssao;
layout(binding = 4, rgba16f) uniform writeonly image2D result;
layout(binding = 15) uniform sampler2D depthMap;

uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform mat4 lightSpaceMatrix;
uniform bool useSSAO;

const vec3 k_a = vec3(0.09f, 0.77f, 0.97f);
const float ambientStrength = 0.2f;
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

#define TREE_K_S_FLAG 1.f
#define LIGHTSOURCE_FLAG 2.f

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

vec4 reflection_Calculation() {
    float maxDistance = 10;
    float resolution  = 0.3;
    int   steps       = 10;
    float thickness   = 0.001;
    
    vec2 texSize  = imageSize(gPosition).xy;
    vec2 texCoord = gl_FragCoord.xy;

    vec4 uv = vec4(0.0);

    //vec4 positionFrom     = texture(positionTexture, texCoord);
    vec4 positionFrom     = imageLoad(gPosition, ivec2(texCoord));
    vec3 unitPositionFrom = normalize(positionFrom.xyz);
    vec3 normal           = normalize(imageLoad(gNormal, ivec2(texCoord)).rgb);
    vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

    vec4 positionTo = positionFrom;

    vec4 startView = vec4(positionFrom.xyz + (pivot *           0), 1);
    vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1);

    vec4 startFrag      = startView;
       // Project to screen space.
    startFrag      = lightSpaceMatrix * startFrag;
       // Perform the perspective divide.
    startFrag.xyz /= startFrag.w;
        // Convert the screen-space XY coordinates to UV coordinates.
    startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       // Convert the UV coordinates to fragment/pixel coordnates.
    startFrag.xy  *= texSize;

    vec4 endFrag      = endView;
    endFrag      = lightSpaceMatrix * endFrag;
    endFrag.xyz /= endFrag.w;
    endFrag.xy   = endFrag.xy * 0.5 + 0.5;
    endFrag.xy  *= texSize;

    vec2 frag  = startFrag.xy;
    uv.xy = frag / texSize;

    float deltaX    = endFrag.x - startFrag.x;
    float deltaY    = endFrag.y - startFrag.y;

    float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);

    vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float viewDistance = startView.z; //might need to be changed to z dimension
    float depth        = thickness;

    float i = 0;

    for (i = 0; i < int(delta); ++i) {
        frag      += increment;
        uv.xy      = frag / texSize;
        positionTo = imageLoad(gPosition, ivec2(frag));

        search1 = mix( (frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);

        search1 = clamp(search1, 0.0, 1.0);

        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
        depth        = viewDistance - positionTo.z;

        if (depth > 0 && depth < thickness) {
            hit0 = 1;
            break;
        } else {
            search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2);

    steps *= hit0;

    for (i = 0; i < steps; ++i) {
        frag       = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy      = frag / texSize;
        positionTo = imageLoad(gPosition, ivec2(frag));

        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
        depth        = viewDistance - positionTo.z;

        if (depth > 0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float value1 = ( 1 - max( dot(-unitPositionFrom, pivot), 0));
    float value2 = ( 1- clamp( depth / thickness, 0, 1));
    float value3 = ( 1- clamp(length(positionTo - positionFrom)/ maxDistance, 0, 1));
    float value4 = (uv.x < 0 || uv.x > 1 ? 0 : 1);
    float value5 = (uv.y < 0 || uv.y > 1 ? 0 : 1);

    float visibility = hit1 * positionTo.w * value1 * value2 * value3 * value4 * value5;

    visibility = clamp(visibility, 0, 1);

    uv.ba = vec2(visibility);

    return uv;
    

}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec3 worldPosition = imageLoad(gPosition, ivec2(fragCoord)).rgb;
    vec4 lightSpacePosition = lightSpaceMatrix * vec4(worldPosition, 1.0);
    vec3 N = imageLoad(gNormal, ivec2(fragCoord)).rgb;
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));
    vec3 L = normalize(lightDir);
    vec4 albedoSpec = imageLoad(gAlbedoSpec, ivec2(fragCoord)).rgba;
    vec3 k_d = albedoSpec.rgb;
    vec3 k_s = vec3(0.1f);
    //check for trees
    if (albedoSpec.a == TREE_K_S_FLAG) {
        // 1 / 0.8 * 0.2 = 0.25
        k_s = 0.25f * k_d;
    }

    float shadow = ShadowCalculation(lightSpacePosition);
    float ambientOcclusion = useSSAO ? imageLoad(ssao, ivec2(fragCoord)).r : 1.f;

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    //ambient: https://learnopengl.com/Lighting/Basic-Lighting
    vec3 ambient = ambientOcclusion * k_a * ambientStrength * k_d;
    vec3 sum = (1.0 - 0.85 * shadow) * (diffuse + specular) + ambient;

    if (albedoSpec.a == LIGHTSOURCE_FLAG) {
        fragColor = vec4(k_d, 0.0);
        //imageStore(result, ivec2(fragCoord), vec4(k_d, 0.0));
    } else {
        fragColor = vec4(sum , 1.0);
        //imageStore(result, ivec2(fragCoord), vec4(sum , 1.0));
    }
    //fragColor = reflection_Calculation();
}
