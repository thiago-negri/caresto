#version 430 core

in vec2 vs_texture;

out vec4 FragColor;

layout (binding = 0) uniform sampler2D g_texture;

void main() {
    FragColor = texelFetch(g_texture, ivec2(vs_texture), 0);
    if (FragColor.a == 0.0f) {
        discard;
    }
}
