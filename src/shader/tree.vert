#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aMaterial;
//layout (location = 3) in vec3 aOffset;

layout (location = 3) in mat4 instanceMatrix;


out vec2 TexCoords;
out vec3 Color;



uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 planet_info;

const float water_Level = planet_info[0];
const float mountain_Height = planet_info[1];
const float snow_peak_Height = planet_info[2];

void main()
{
    vec3 worldPosition = vec3(instanceMatrix * vec4(1.0f));
    float distance_To_Planet_Center =length(worldPosition);
    if(distance_To_Planet_Center > water_Level && distance_To_Planet_Center < snow_peak_Height) {

        if(aMaterial == 0.0f) {
            Color = vec3(80.0f/255.f,122.f/255.f,73.f/255.f);
        } else {
            Color = vec3(126.f/255.f,86.f/255.f,52.f/255.f);
        }
        TexCoords = vec2(1.0f,1.0f);
        gl_Position = projection * view * instanceMatrix * vec4(aPos,1.0f);
    } else {
        gl_Position = projection * view * mat4(0.0f) * vec4(aPos,1.0f);
    }   
}