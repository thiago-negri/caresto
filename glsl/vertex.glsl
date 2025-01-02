#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;

out VS_OUT { vec2 size; } vs_out;

void main() {
    gl_Position = vec4(position.xy, 0.0, 1.0);
    vs_out.size = size;
}
