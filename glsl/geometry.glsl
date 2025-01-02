#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT { vec2 size; } gs_in[];

void main() {
    vec4 pos = gl_in[0].gl_Position;
    vec2 size = gs_in[0].size;

    // top left
    gl_Position = pos;
    EmitVertex();

    // bottom left
    gl_Position = pos + vec4(0.0, size.y, 0.0, 0.0);
    EmitVertex();

    // bottom right
    gl_Position = pos + vec4(size.xy, 0.0, 0.0);
    EmitVertex();

    // bottom right
    gl_Position = pos + vec4(size.xy, 0.0, 0.0);
    EmitVertex();

    // top right
    gl_Position = pos + vec4(size.x, 0.0, 0.0, 0.0);
    EmitVertex();

    // top left
    gl_Position = pos;
    EmitVertex();
}
