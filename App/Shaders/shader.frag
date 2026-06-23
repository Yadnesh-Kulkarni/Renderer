#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    int isWireframe;
} pc;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 color = pc.isWireframe != 0 ? vec3(0.0) : fragColor;
    outColor = vec4(color, 1.0);
}