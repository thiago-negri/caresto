#include <engine/el_log.h>
#include <engine/et_test.h>
#include <engine/eu_utils.h>
#include <stdio.h>
#include <sys/stat.h>

int eu_copy_file(const char *from, const char *to, struct em_arena *arena) {
    int rc = 0;
    FILE *fd = NULL;

    size_t arena_offset = em_arena_save_offset(arena);

    errno_t err = fopen_s(&fd, from, "rb");
    if (err != 0) {
        el_critical("U: Could not open for read: %s.\n", from);
        rc = -1;
        goto _err;
    }
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // +1 for NULL terminator
    char *buffer = (char *)em_arena_alloc(arena, file_size + 1);
    if (buffer == NULL) {
        el_critical("WIN: OOM: Could not allocate to read file %s.\n", from);
        rc = -1;
        goto _err;
    }
    memset(buffer, 0, file_size + 1);
    size_t read_count = fread(buffer, sizeof(char), file_size, fd);
    if (read_count != (size_t)file_size) {
        el_critical("U: Something went wrong while reading %s.\n", from);
        rc = -1;
        goto _err;
    }

    fclose(fd);
    fd = 0;

    err = fopen_s(&fd, to, "wb");
    if (err != 0) {
        el_critical("U: Could not open for write: %s.\n", to);
        rc = -1;
        goto _err;
    }

    size_t write_count = fwrite(buffer, sizeof(char), file_size, fd);
    if (write_count != (size_t)file_size) {
        el_critical("WIN: Something went wrong while writing %s.\n", to);
        rc = -1;
        goto _err;
    }

    fclose(fd);
    goto _done;

_err:
    if (fd != NULL) {
        fclose(fd);
    }

_done:
    em_arena_restore_offset(arena, arena_offset);
    return rc;
}

long long eu_file_timestamp(const char *path) {
    struct stat file_stat = {0};
    stat(path, &file_stat);
    long long timestamp = file_stat.st_mtime;
    return timestamp;
}

long long eu_max(long long a, long long b, long long c) {
    return a > b ? a > c ? a : c : b > c ? b : c;
}

ET_TEST(eu_max) {
    ET_ASSERT(eu_max(1, 2, 3) == 3);
    ET_ASSERT(eu_max(2, 1, 3) == 3);
    ET_ASSERT(eu_max(1, 3, 2) == 3);
    ET_ASSERT(eu_max(2, 3, 1) == 3);
    ET_ASSERT(eu_max(3, 1, 2) == 3);
    ET_ASSERT(eu_max(3, 2, 1) == 3);
    ET_DONE;
}

float eu_lerp(float start, float end, float d) {
    return start + (end - start) * d;
}

void eu_mat4_identity(struct eu_mat4 *out) {
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

void eu_mat4_ortho_camera(struct eu_mat4 *out, float w, float h, float x,
                          float y) {
    float half_width = w * 0.5f;
    float half_height = h * 0.5f;
    float top = y - half_height;
    float left = x - half_width;
    float right = x + half_width;
    float bottom = y + half_height;
    float near = 0.0f;
    float far = 1.0f;
    eu_mat4_ortho(out, left, right, top, bottom, near, far);
}

void eu_mat4_ortho(struct eu_mat4 *out, float left, float right, float top,
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
    struct eu_mat4 a = {};
    eu_mat4_ortho(&a, 0.0f, 360.0f, 0.0f, 640.0f, 0.0f, 1.0f);
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

void eu_ivec2_diff(struct eu_vec2 *value, struct eu_vec2 *subtract,
                   struct eu_ivec2 *out) {
    out->x = value->x - subtract->x;
    out->y = value->y - subtract->y;
}
