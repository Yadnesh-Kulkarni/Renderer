#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    int isWireframe;
} pc;

layout(set = 0, binding = 0) uniform sampler2D baseColorTex;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 color = pc.isWireframe != 0
        ? vec3(0.0)
        : texture(baseColorTex, fragTexCoord).rgb;
    outColor = vec4(color, 1.0);
}
