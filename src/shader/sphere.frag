#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
in vec2 TexCoord;
flat in int inCrater;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform sampler2D grassland;
uniform sampler2D snow;
uniform sampler2D mountain;
uniform sampler2D water;

const vec3 k_s = vec3(0.1f);
const float n = 100.0f;
const vec3 light_intensity = vec3(20.0f);

const float water_Level = 1.4f;
const float mountain_Height = 1.7f;
const float snow_peak_Height = 1.78f;
const float ocean_interpolation_width = 0.1f;
const float mountain_interpolation_width = 0.03f;
const float snow_interpolation_width = 0.01f;

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPos - worldPosition);
    vec3 R = normalize(reflect((-1)*V, N));

    vec3 L = normalize(cameraPos - worldPosition);

    vec3 k_d;

    //TODO interpolate texture at borderlines
    //TODO calculate height at which texture should load dynamically
    float distance_To_Planet_Center = length(worldPosition);
    float distance_To_Biome_Edge;
    float interpolation_degree;
    if(inCrater == 1) {
        k_d = vec3(0.5, 0.0, 0.0);
    } else {
        if(distance_To_Planet_Center > water_Level) {
            distance_To_Biome_Edge = distance_To_Planet_Center - water_Level;
            interpolation_degree = distance_To_Biome_Edge / ocean_interpolation_width;
            if(interpolation_degree <= 0.7f) {
                k_d = mix(texture(water,TexCoord).rgb,texture(grassland,TexCoord).rgb, interpolation_degree);
            } else {
            k_d = texture(grassland,TexCoord).rgb;
            }
        } else {
            k_d = texture(water,TexCoord).rgb;
        }
        if(distance_To_Planet_Center > mountain_Height) {
            distance_To_Biome_Edge = distance_To_Planet_Center - mountain_Height;
            interpolation_degree = distance_To_Biome_Edge / mountain_interpolation_width;
            if(interpolation_degree <= 0.9f) {
                k_d = mix(texture(grassland,TexCoord).rgb,texture(mountain,TexCoord).rgb, interpolation_degree);
            } else {
            k_d = texture(mountain,TexCoord).rgb;
            }
        }
        if(distance_To_Planet_Center > snow_peak_Height) {
            distance_To_Biome_Edge = distance_To_Planet_Center - snow_peak_Height;
            interpolation_degree = distance_To_Biome_Edge / snow_interpolation_width;
            if(interpolation_degree <= 1.0f) {
                k_d = mix(texture(mountain,TexCoord).rgb,texture(snow,TexCoord).rgb, interpolation_degree);
            } else {
            k_d = texture(snow,TexCoord).rgb;
            }
        }
    }

    vec3 diffuse = k_d * max(0.0, dot(L, N));
    vec3 specular = k_s *  pow(max(0.0, dot(R, L)), n);
    vec3 sum = diffuse + specular;

/* leave out for now, gets too bright when close to the sphere*/
//     float distance = dot(worldPosition - cameraPos, worldPosition - cameraPos);
//     vec3 intensity = light_intensity / distance;
    fragColor = vec4(sum /** intensity*/, 1.0);
}
