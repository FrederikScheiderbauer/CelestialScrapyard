#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aOffset;


void main()
{
    gl_Position = vec4(aPos + aOffset, 1.0);

} 