#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec4 outColor;

layout(binding = 0) uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

void main() {
    outUv = uv;
    outColor = color;
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(position, 1.0);
}