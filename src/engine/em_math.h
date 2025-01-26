#pragma once

#include <engine/ea_allocator.h>
#include <stddef.h>

struct em_ixpos {
    size_t x, y;
};

struct em_ixpos_2x {
    size_t x1, x2, y;
};

struct em_ipos {
    int x, y;
};

struct em_ipos_2x {
    int x1, x2, y;
};

struct em_itexpos {
    int u, v;
};

struct em_fpos {
    float x, y;
};

struct em_isize {
    int w, h;
};

struct em_vec2 {
    float x, y;
};

struct em_ivec2 {
    int x, y;
};

struct em_vec4 {
    float x, y, z, w;
};

struct em_color {
    float r, g, b, a;
};

struct em_mat4 {
    float ax, bx, cx, dx;
    float ay, by, cy, dy;
    float az, bz, cz, dz;
    float aw, bw, cw, dw;
};

long long em_max(long long a, long long c);

float em_lerp(float start, float end, float d);

void em_mat4_ortho_camera(struct em_mat4 *out, float w, float h, float x,
                          float y);

void em_mat4_ortho(struct em_mat4 *out, float left, float right, float top,
                   float bottom, float near, float far);

void em_mat4_identity(struct em_mat4 *out);

void em_ivec2_diff(struct em_vec2 *value, struct em_vec2 *subtract,
                   struct em_ivec2 *out);

int em_sign(int value);
