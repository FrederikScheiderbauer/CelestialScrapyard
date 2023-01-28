#version 460
out vec4 fragColor;
layout(binding = 0, rgba16f) uniform readonly image2D lightingPassResult;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    fragColor = imageLoad(lightingPassResult, ivec2(fragCoord));
}