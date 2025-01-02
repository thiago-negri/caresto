#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

void main() {
    // top left
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    // bottom left
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.1, 0.0, 0.0);
    EmitVertex();

    // bottom right
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
    EmitVertex();

    // bottom right
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
    EmitVertex();

    // top right
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    // top left
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
}
