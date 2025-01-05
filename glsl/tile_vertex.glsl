#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in ivec2 texture;

out VS_OUT { ivec2 texture; } vs_out;

uniform ivec2 g_tile_size;

void main() {
    gl_Position = vec4(position.xy * g_tile_size, 0.0, 1.0);
    vs_out.texture = texture;
}
