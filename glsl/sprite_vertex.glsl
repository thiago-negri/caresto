#version 430 core

layout (location = 0) in ivec2 position;
layout (location = 1) in ivec2 texture;

out vec2 vs_texture;

uniform mat4 g_transform_mat;

void main() {
    gl_Position = g_transform_mat * vec4(position.xy, 0.0, 1.0);
    vs_texture = texture;
}
