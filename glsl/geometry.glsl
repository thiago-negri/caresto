#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT { ivec2 size; ivec2 texture; } gs_in[];

out vec2 f_texture;

uniform mat4 g_transform_mat;

void main() {
    vec4 pos = gl_in[0].gl_Position;
    ivec2 size = gs_in[0].size;
    ivec2 texture = gs_in[0].texture;

    // top left
    gl_Position = g_transform_mat * pos;
    f_texture = texture;
    EmitVertex();

    // bottom left
    gl_Position = g_transform_mat * (pos + vec4(0.0, -1.0 * size.y, 0.0, 0.0));
    f_texture = texture + ivec2(0, size.y);
    EmitVertex();

    // bottom right
    gl_Position = g_transform_mat * (pos + vec4(size.x, -1.0 * size.y, 0.0, 0.0));
    f_texture = texture + size.xy;
    EmitVertex();

    // bottom right
    gl_Position = g_transform_mat * (pos + vec4(size.x, -1.0 * size.y, 0.0, 0.0));
    f_texture = texture + size.xy;
    EmitVertex();

    // top right
    gl_Position = g_transform_mat * (pos + vec4(size.x, 0.0, 0.0, 0.0));
    f_texture = texture + ivec2(size.x, 0);
    EmitVertex();

    // top left
    gl_Position = g_transform_mat * pos;
    f_texture = texture;
    EmitVertex();
}
