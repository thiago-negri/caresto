#ifndef EU_UTILS_H
#define EU_UTILS_H

#include <engine/el_log.h>
#include <engine/em_memory.h>

struct eu_ixpos {
    size_t x, y;
};

struct eu_ixpos_2x {
    size_t x1, x2, y;
};

struct eu_ipos {
    int x, y;
};

struct eu_ipos_2x {
    int x1, x2, y;
};

struct eu_itexpos {
    int u, v;
};

struct eu_fpos {
    float x, y;
};

struct eu_isize {
    int w, h;
};

struct eu_vec2 {
    float x, y;
};

struct eu_ivec2 {
    int x, y;
};

struct eu_vec4 {
    float x, y, z, w;
};

struct eu_mat4 {
    float ax, bx, cx, dx;
    float ay, by, cy, dy;
    float az, bz, cz, dz;
    float aw, bw, cw, dw;
};

int eu_copy_file(const char *from, const char *to, struct em_arena *arena);

long long eu_file_timestamp(const char *path);

long long eu_max(long long a, long long b, long long c);

float eu_lerp(float start, float end, float d);

void eu_mat4_ortho_camera(struct eu_mat4 *out, float w, float h, float x,
                          float y);

void eu_mat4_ortho(struct eu_mat4 *out, float left, float right, float top,
                   float bottom, float near, float far);

void eu_mat4_identity(struct eu_mat4 *out);

void eu_ivec2_diff(struct eu_vec2 *value, struct eu_vec2 *subtract,
                   struct eu_ivec2 *out);

#define eu_assert(x)                                                           \
    do {                                                                       \
        if (!(x)) {                                                            \
            el_critical("assertion %s:%d %s\n", __FILE__, __LINE__, #x);       \
            SDL_TriggerBreakpoint();                                           \
        }                                                                      \
    } while (0)

#endif // EU_UTILS_H
