#ifndef CT_TILEMAP_H
#define CT_TILEMAP_H

#include <caresto/cc_camera.h>
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
    int tile_count;
    struct egl_tile tiles[CT_TILES_MAX];
    struct ct_tile tilemap[CT_TILEMAP_MAX_HEIGHT][CT_TILEMAP_MAX_WIDTH];
};

void ct_set(struct ct_tilemap *tilemap, int x, int y,
            enum ct_tile_type tile_type);

void ct_screen_pos(struct eu_ipos *pos, struct cc_bounds *cam_bounds,
                   struct eu_isize *win_size, struct eu_fpos *win_pos,
                   struct eu_isize *tile_size);

void ct_game_pos(struct eu_ixpos *pos, struct eu_ipos *game_pos,
                 struct eu_isize *tile_size);

void ct_game_pos_2x(struct eu_ixpos_2x *pos, struct eu_ipos_2x *game_pos,
                    struct eu_isize *tile_size);

#endif // CT_TILEMAP_H
