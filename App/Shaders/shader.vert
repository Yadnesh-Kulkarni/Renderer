#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    int isWireframe;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
    fragColor = abs(inNormal) + vec3(inTexCoord, 0.0) * 0.0;
}
