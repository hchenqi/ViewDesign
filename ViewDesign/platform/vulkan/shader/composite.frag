#version 450

layout(push_constant) uniform Context {
    layout(offset = 32) float opacity;
} context;

layout(set = 0, binding = 0) uniform sampler2D src;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 color;

void main() {
    color = texture(src, uv) * context.opacity;
}
