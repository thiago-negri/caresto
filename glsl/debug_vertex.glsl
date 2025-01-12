#version 430 core

layout (location = 0) in ivec2 position;
layout (location = 1) in vec4 color;

out vec4 vs_color;

layout (binding = 0) uniform mat4 g_transform_mat;

void main() {
    gl_Position = vec4(position.xy, 0.0, 1.0) * g_transform_mat;
    vs_color = color;
}
