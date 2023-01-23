#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec3 TexCoord;
flat in int inCrater;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D crater;

uniform samplerCube grassland_cube;
uniform samplerCube snow_cube;
uniform samplerCube mountain_cube;
uniform samplerCube water_cube;
uniform samplerCube crater_cube;

uniform vec3 planet_info;
uniform bool depthRender;

float water_Level = planet_info[0];
float deep_water_Level = planet_info[0] -0.2f;
float mountain_Height = planet_info[1];
float snow_peak_Height = planet_info[2];
float ocean_interpolation_width = 0.1f;
float mountain_interpolation_width = 0.03f;
float snow_interpolation_width = 0.01f;

void main()
{
    if (depthRender) {
        return;
    }

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

    gPosition = worldPosition;
    gNormal = normalize(worldNormal);
    gAlbedoSpec.rgb = k_d;
}
