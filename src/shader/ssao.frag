#version 460
#extension GL_KHR_shader_subgroup_quad : enable
out float fragColor;

layout(binding = 0, rgba16f) uniform image2D gPosition;
layout(binding = 1, rgba16f) uniform image2D gNormal;

#define NUM_SSAO_SAMPLES 64
#define RADIUS 0.5
#define BIAS 0.025

uniform mat4 view;
uniform mat4 projection;
uniform vec3 kernel[NUM_SSAO_SAMPLES];
uniform vec2 resolution;

//https://www.shadertoy.com/view/4djSRW
vec2 hash22(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec3 viewPos = vec3(view * vec4(imageLoad(gPosition, ivec2(fragCoord)).rgb, 1.f));
    vec3 viewNormal = vec3(view * vec4(imageLoad(gNormal, ivec2(fragCoord)).rgb, 0.f));
    vec3 randomDir = vec3(2.f * hash22(fragCoord) - vec2(1.f), 0.f);

    vec3 tangent = normalize(randomDir - viewNormal * dot(randomDir, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_SSAO_SAMPLES; ++i)
    {
        vec3 samplePos = TBN * kernel[i];
        samplePos = viewPos + samplePos * RADIUS;
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5 + 0.5;
        vec2 screenCoords = offset.xy * resolution;

        float sampleDepth = vec3(view * vec4(imageLoad(gPosition, ivec2(screenCoords)).xyz, 1.f)).z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(viewPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / NUM_SSAO_SAMPLES);
    #ifdef GL_KHR_shader_subgroup_quad
    occlusion += subgroupQuadSwapHorizontal(occlusion);
    occlusion += subgroupQuadSwapVertical(occlusion);
    occlusion /= 4.f;
    #endif
    fragColor = occlusion;
}