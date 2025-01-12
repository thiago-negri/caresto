#pragma once

#include <caresto/system/csc_camera.h>
#include <engine/eo_opengl.h>

#define CST_TILEMAP_MAX_WIDTH 100
#define CST_TILEMAP_MAX_HEIGHT 100
#define CST_TILES_MAX 1024

enum cst_tile_type {
    CST_TILE_TYPE_EMPTY = 0,
    CST_TILE_TYPE_GRASS,
    CST_TILE_TYPE_SAND,
    CST_TILE_TYPE_WATER,
};

struct cst_tile {
    enum cst_tile_type type;
    size_t tiles_index;
};

struct cst_tile_map {
    int tile_count;
    struct eo_tile tiles[CST_TILES_MAX];
    struct cst_tile tilemap[CST_TILEMAP_MAX_HEIGHT][CST_TILEMAP_MAX_WIDTH];
};

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type tile_type);

//TODO(tnegri): Add cst_get

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos,
                    struct em_isize *tile_size);

void cst_game_pos(struct em_ixpos *pos, struct em_ipos *game_pos,
                  struct em_isize *tile_size);

void cst_game_pos_2x(struct em_ixpos_2x *pos, struct em_ipos_2x *game_pos,
                     struct em_isize *tile_size);
