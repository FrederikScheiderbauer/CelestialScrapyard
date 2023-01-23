#version 460 core
in vec3 worldNormal;
in vec3 worldPosition;
flat in int instanceId;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform bool picking;
uniform int pickedID;
//0: draw normal, 1: only draw picked asteroid with write to stencil buffer, 2: draw picked with asteroid with solid color
uniform int outlining;
uniform bool depthRender;

vec3 instanceIdToColor() {
    //http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
    float r = float((instanceId & 0x000000FF) >> 0) / 255.f;
    float g = float((instanceId & 0x0000FF00) >> 8) / 255.f;
    float b = float((instanceId & 0x00FF0000) >> 16) / 255.f;
    return vec3(r,g,b);
}

void main()
{
    if (depthRender) {
        return;
    }
    if ((outlining > 0 && pickedID != instanceId) || (outlining == 0 && pickedID == instanceId)) {
        discard;
    }

    if (picking) {
        //gPosition is at out location 0 for deferred shading -> replaces fragColor for picking
        gPosition = instanceIdToColor();
    } else {
        if (outlining == 2) {
            //gPosition is at out location 0 for deferred shading -> replaces fragColor for picking
            gPosition = vec3(0.8, 0.0, 0.0);
        } else {
            gPosition = worldPosition;
            gNormal = normalize(worldNormal);
            gAlbedoSpec.rgb = vec3(0.5);
        }
    }
}
