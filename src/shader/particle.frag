#version 460
layout (location = 0) out vec4 fragColor;
uniform vec4 u_Color;

void main() {
    fragColor = u_Color;
}
