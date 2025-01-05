#version 430 core

in vec2 f_texture;

out vec4 FragColor;

layout (binding = 0) uniform sampler2D g_tile_atlas;

void main() {
    FragColor = texelFetch(g_tile_atlas, ivec2(f_texture), 0);
    if (FragColor.a == 0.0f) {
        discard;
    }
}
