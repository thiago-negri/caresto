#ifndef EU_UTILS_H
#define EU_UTILS_H

#include <engine/em_memory.h>

struct eu_vec2 {
    union {
        float values[2];
        struct {
            float x, y;
        };
        struct {
            float u, v;
        };
        struct {
            float w, h;
        };
    };
};

struct eu_ivec2 {
    union {
        int values[2];
        struct {
            int x, y;
        };
        struct {
            int u, v;
        };
        struct {
            int w, h;
        };
    };
};

struct eu_vec4 {
    union {
        float values[4];
        struct {
            float x, y, z, w;
        };
    };
};

struct eu_mat4 {
    union {
        float values[16];
        struct {
            float ax, bx, cx, dx;
            float ay, by, cy, dy;
            float az, bz, cz, dz;
            float aw, bw, cw, dw;
        };
    };
};

int eu_copy_file(const char *from, const char *to, struct em_arena *arena);

long long eu_file_timestamp(const char *path);

long long eu_max(long long a, long long b, long long c);

float eu_lerp(float start, float end, float d);

void eu_mat4_ortho(struct eu_mat4 *out, float left, float right, float top,
                   float bottom, float near, float far);

void eu_mat4_identity(struct eu_mat4 *out);

void eu_ivec2_diff(struct eu_vec2 *value, struct eu_vec2 *subtract,
                   struct eu_ivec2 *out);

// TODO(tnegri): Create assertions
#define eu_assert(X)                                                           \
    if (X) {                                                                   \
    }

#endif // EU_UTILS_H
