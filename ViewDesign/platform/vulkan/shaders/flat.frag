#version 450

layout(push_constant) uniform Context {
    layout(offset = 32) vec4 color;
} context;

layout(location = 0) out vec4 color;

void main() {
    color = context.color;
}
