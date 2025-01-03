#version 430 core

in vec2 f_texture;

out vec4 FragColor;

uniform sampler2D g_sprite_atlas;

void main() {
    FragColor = texelFetch(g_sprite_atlas, ivec2(f_texture), 0);
}
