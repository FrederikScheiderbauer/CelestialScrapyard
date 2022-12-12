#version 460
layout (location = 0) out vec4 fragColor;
uniform vec4 u_Color;

void main() {
    fragColor = vec4(u_Color.rgb, 0.5f);
}
