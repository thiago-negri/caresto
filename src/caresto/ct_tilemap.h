#ifndef CT_TILEMAP_H
#define CT_TILEMAP_H

#include <engine/egl_opengl.h>

#define CT_TILEMAP_MAX_WIDTH 100
#define CT_TILEMAP_MAX_HEIGHT 100
#define CT_TILES_MAX 1024

enum ct_tile_type {
    CT_TILE_TYPE_EMPTY = 0,
    CT_TILE_TYPE_GRASS,
    CT_TILE_TYPE_SAND,
    CT_TILE_TYPE_WATER,
};

struct ct_tile {
    enum ct_tile_type type;
    size_t tiles_index;
};

struct ct_tilemap {
    size_t tile_count;
    struct egl_tile tiles[CT_TILES_MAX];
    struct ct_tile tilemap[CT_TILEMAP_MAX_HEIGHT][CT_TILEMAP_MAX_WIDTH];
};

void ct_set(struct ct_tilemap *tilemap, size_t x, size_t y,
            enum ct_tile_type tile_type);

void ct_screen_pos(GLfloat screen_left, GLfloat screen_right,
                   GLfloat screen_top, GLfloat screen_bottom, GLfloat screen_w,
                   GLfloat screen_h, GLfloat screen_x, GLfloat screen_y,
                   int tile_size_w, int tile_size_h, size_t *out_tile_x,
                   size_t *out_tile_y);

void ct_game_pos(struct eu_ivec2 *pos, int tile_size_w, int tile_size_h,
                 size_t *out_tile_x, size_t *out_tile_y);

#endif // CT_TILEMAP_H
