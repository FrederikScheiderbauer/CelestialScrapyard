#version 460

layout (location = 0) in vec3 a_Position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec3 position;
uniform float size;

void main()
{
    //Billboarding: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
    vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPosition_worldspace = position + CameraRight_worldspace * a_Position.x * size + CameraUp_worldspace * a_Position.y * size;

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);
}
