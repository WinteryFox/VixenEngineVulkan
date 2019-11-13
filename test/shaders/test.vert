#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0)
);

layout(location = 0) out mat4 projection;
layout(location = 5) out mat4 view;

layout(binding = 0) uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

void main() {
    projection = mvp.projection;
    view = mvp.view;
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}