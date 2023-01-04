#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aMaterial;
layout (location = 3) in vec3 aOffset;

out vec2 TexCoords;
out vec3 Color;



uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    vec3 offset = aOffset;
    vec4 worldPosition_2 = model * vec4(aPos,1.0f) + vec4(aOffset,1.0f);
    vec4 worldPosition = vec4(aPos+aOffset,1.0f);
    //mat4 orient_from_origin = 

    if(aMaterial == 0.0f) {
        Color = vec3(80.0f/255.f,122.f/255.f,73.f/255.f);
    } else {
        Color = vec3(126.f/255.f,86.f/255.f,52.f/255.f);
    }
    TexCoords = vec2(1.0f,1.0f);
    gl_Position = projection * view * worldPosition_2;
    //gl_Position = vec4(aPos,1.0f);
}