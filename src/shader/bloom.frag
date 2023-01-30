#version 460
out vec4 fragColor;
layout(binding = 0, rgba16f) uniform readonly image2D readBuffer;
layout(binding = 1, rgba16f) uniform writeonly image2D writeBuffer;
uniform bool lastPass;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
//    ivec2 offset[8];
//    offset[0] = ivec2(-1, 1);
//    offset[1] = ivec2(-1, 0);
//    offset[2] = ivec2(-1, -1);
//    offset[3] = ivec2(0, 1);
//    offset[4] = ivec2(0, -1);
//    offset[5] = ivec2(1, 1);
//    offset[6] = ivec2(1, 0);
//    offset[7] = ivec2(1, -1);
//
//    vec4 center = imageLoad(readBuffer, ivec2(fragCoord));
//    vec4 blur = center;
//    for (int i = 0; i < 8; ++i) {
//        blur += imageLoad(readBuffer, ivec2(fragCoord) + offset[i]);
//    }
//
//    vec4 result;
//    // light source has alpha = 0 -> if result alpha does not add to 1 at least one pixel contained the light source
//    if (abs(blur.a - 9.0) >= 1E-5) {
//        result = blur / 9.0;
//    } else {
//        result = center;
//    }

    fragColor = imageLoad(readBuffer, ivec2(fragCoord));

//    if (lastPass) {
//        fragColor = result;
//    } else {
//        imageStore(writeBuffer, ivec2(fragCoord), result);
//    }
}