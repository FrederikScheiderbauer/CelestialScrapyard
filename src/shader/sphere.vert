#version 460 core
layout (location = 0) in vec3 position;
out vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    //Currently, this just a unit sphere, so the normal n = p - o is just p because o = (0,0)
    normal = position;
}
