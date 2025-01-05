#version 430 core

layout (location = 0) in ivec2 position;
layout (location = 1) in ivec2 size;
layout (location = 2) in ivec2 texture;

out VS_OUT { ivec2 size; ivec2 texture; } vs_out;

void main() {
    gl_Position = vec4(position.xy, 0.0, 1.0);
    vs_out.size = size;
    vs_out.texture = texture;
}
