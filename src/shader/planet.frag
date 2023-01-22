#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec3 TexCoord;
in vec4 lightSpacePosition;
flat in int inCrater;
//out vec4 fragColor;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;


uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform sampler2D crater;

uniform samplerCube grassland_cube;
uniform samplerCube snow_cube;
uniform samplerCube mountain_cube;
uniform samplerCube water_cube;
uniform samplerCube crater_cube;

layout(binding = 15) uniform sampler2D depthMap;

const vec3 k_s = vec3(0.1f);
const vec3 k_a = vec3(0.09f, 0.77f, 0.97f);
const float ambientStrength = 0.2f;
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);


uniform vec3 planet_info;
uniform bool depthRender;

float water_Level = planet_info[0];
float deep_water_Level = planet_info[0] -0.2f;
float mountain_Height = planet_info[1];
float snow_peak_Height = planet_info[2];
float ocean_interpolation_width = 0.1f;
float mountain_interpolation_width = 0.03f;
float snow_interpolation_width = 0.01f;

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
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));

    vec3 L = normalize(lightPos - worldPosition);

    vec3 k_d;

    //TODO interpolate texture at borderlines
    //TODO calculate height at which texture should load dynamically
    float distance_To_Planet_Center = length(worldPosition);
    float distance_To_Biome_Edge;
    float interpolation_degree;


    if(inCrater == 1) {
        k_d =  texture(crater,vec2(TexCoord)).rgb;
    } else {
        if(distance_To_Planet_Center < water_Level) {
            distance_To_Biome_Edge = water_Level - distance_To_Planet_Center;
            interpolation_degree = distance_To_Biome_Edge / ocean_interpolation_width;
            if(interpolation_degree <= 0.8f) {
                k_d = mix(texture(grassland_cube,TexCoord).rgb,texture(water_cube,TexCoord).rgb, interpolation_degree);
            } else {
                k_d = texture(water_cube,TexCoord).rgb;
            }
        } else {
            k_d = texture(grassland_cube,TexCoord).rgb;
        }
        if(distance_To_Planet_Center > mountain_Height) {
            distance_To_Biome_Edge = distance_To_Planet_Center - mountain_Height;
            interpolation_degree = distance_To_Biome_Edge / mountain_interpolation_width;
            if(interpolation_degree <= 0.9f) {
                k_d = mix(texture(grassland_cube,TexCoord).rgb,texture(mountain_cube,TexCoord).rgb, interpolation_degree);
            } else {
            k_d = texture(mountain_cube,TexCoord).rgb;
            }
        }
        if(distance_To_Planet_Center > snow_peak_Height) {
            distance_To_Biome_Edge = distance_To_Planet_Center - snow_peak_Height;
            interpolation_degree = distance_To_Biome_Edge / snow_interpolation_width;
            if(interpolation_degree <= 1.0f) {
                k_d = mix(texture(mountain_cube,TexCoord).rgb,texture(snow_cube,TexCoord).rgb, interpolation_degree);
            } else {
            k_d = texture(snow_cube,TexCoord).rgb;
            }
        }
        if(distance_To_Planet_Center < deep_water_Level) {
            distance_To_Biome_Edge = deep_water_Level - distance_To_Planet_Center;
            interpolation_degree = distance_To_Biome_Edge / ocean_interpolation_width;
            if(interpolation_degree <= 1.0f) {
                k_d = mix(texture(water_cube,TexCoord).rgb ,texture(water_cube,TexCoord).rgb*0.6f, interpolation_degree);
            } else {
                k_d = texture(water_cube,TexCoord).rgb*0.6f;
            }
        }
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
    gAlbedoSpec.a = 1.0f;
//    // store specular intensity in gAlbedoSpec's alpha component
//    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;

/* leave out for now, gets too bright when close to the sphere*/
//     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
//     vec3 intensity = light_intensity / distance;
    //fragColor = vec4(sum /** intensity*/, 1.0);
}
