#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(push_constant) uniform PC {
    mat4 mvp;
} pc;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = pc.mvp * vec4(inPos, 1.0);
    fragColor = inColor;
}
