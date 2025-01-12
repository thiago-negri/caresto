#include <engine/el_log.h>
#include <engine/em_math.h>
#include <engine/et_test.h>

long long em_max(long long a, long long b, long long c) {
    return a > b ? a > c ? a : c : b > c ? b : c;
}

ET_TEST(em_max) {
    ET_ASSERT(em_max(1, 2, 3) == 3);
    ET_ASSERT(em_max(2, 1, 3) == 3);
    ET_ASSERT(em_max(1, 3, 2) == 3);
    ET_ASSERT(em_max(2, 3, 1) == 3);
    ET_ASSERT(em_max(3, 1, 2) == 3);
    ET_ASSERT(em_max(3, 2, 1) == 3);
    ET_DONE;
}

float em_lerp(float start, float end, float d) {
    return start + (end - start) * d;
}

void em_mat4_identity(struct em_mat4 *out) {
    out->ax = 1.0f;
    out->ay = 0.0f;
    out->az = 0.0f;
    out->aw = 0.0f;
    out->bx = 0.0f;
    out->by = 1.0f;
    out->bz = 0.0f;
    out->bw = 0.0f;
    out->cx = 0.0f;
    out->cy = 0.0f;
    out->cz = 1.0f;
    out->cw = 0.0f;
    out->dx = 0.0f;
    out->dy = 0.0f;
    out->dz = 0.0f;
    out->dw = 1.0f;
}

void em_mat4_ortho_camera(struct em_mat4 *out, float w, float h, float x,
                          float y) {
    float half_width = w * 0.5f;
    float half_height = h * 0.5f;
    float top = y - half_height;
    float left = x - half_width;
    float right = x + half_width;
    float bottom = y + half_height;
    float near = 0.0f;
    float far = 1.0f;
    em_mat4_ortho(out, left, right, top, bottom, near, far);
}

void em_mat4_ortho(struct em_mat4 *out, float left, float right, float top,
                   float bottom, float near, float far) {
    out->ax = 2.0f / (right - left);
    out->ay = 0.0f;
    out->az = 0.0f;
    out->aw = -1.0f * (right + left) / (right - left);
    out->bx = 0.0f;
    out->by = -2.0f / (bottom - top);
    out->bz = 0.0f;
    out->bw = (bottom + top) / (bottom - top);
    out->cx = 0.0f;
    out->cy = 0.0f;
    out->cz = -2.0f / (far - near);
    out->cw = -1.0f * (far + near) / (far - near);
    out->dx = 0.0f;
    out->dy = 0.0f;
    out->dz = 0.0f;
    out->dw = 1.0f;
}

ET_TEST(mat4_ortho) {
    struct em_mat4 a = {};
    em_mat4_ortho(&a, 0.0f, 360.0f, 0.0f, 640.0f, 0.0f, 1.0f);
    ET_ASSERT(a.ax >= 0.005555f && a.ax <= 0.005557f);
    ET_ASSERT(a.ay == 0.0f);
    ET_ASSERT(a.az == 0.0f);
    ET_ASSERT(a.aw == -1.0f);
    ET_ASSERT(a.bx == 0.0f);
    ET_ASSERT(a.by >= -0.003126f && a.by <= -0.003124f);
    ET_ASSERT(a.bz == 0.0f);
    ET_ASSERT(a.bw == 1.0f);
    ET_ASSERT(a.cx == 0.0f);
    ET_ASSERT(a.cy == 0.0f);
    ET_ASSERT(a.cz == -2.0f);
    ET_ASSERT(a.cw == -1.0f);
    ET_ASSERT(a.dx == 0.0f);
    ET_ASSERT(a.dy == 0.0f);
    ET_ASSERT(a.dz == 0.0f);
    ET_ASSERT(a.dw == 1.0f);
    ET_DONE;
}

void em_ivec2_diff(struct em_vec2 *value, struct em_vec2 *subtract,
                   struct em_ivec2 *out) {
    out->x = value->x - subtract->x;
    out->y = value->y - subtract->y;
}
