#version 450

layout(push_constant) uniform Context {
    vec2 size;
	float transform[6];
} context;

layout(location = 0) in vec2 point;

vec2 transform(vec2 point) {
    point = vec2(
        context.transform[0] * point.x + context.transform[2] * point.y + context.transform[4],
        context.transform[1] * point.x + context.transform[3] * point.y + context.transform[5]
    );
    return (point / context.size) * 2.0 - 1.0;
}

void main() {
	gl_Position = vec4(transform(point), 0.0, 1.0);
}
