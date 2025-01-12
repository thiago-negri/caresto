#pragma once

#include <caresto/system/csc_camera.h>
#include <engine/eo_opengl.h>

#define CST_TILEMAP_MAX_WIDTH 100
#define CST_TILEMAP_MAX_HEIGHT 100
#define CST_TILES_MAX 1024

enum cst_tile_type : unsigned char {
    CST_EMPTY,
    CST_SOLID,
};

struct cst_tile {
    enum cst_tile_type type;
    int tiles_index;
};

struct cst_tile_map {
    int tile_count;
    struct eo_tile tiles[CST_TILES_MAX];
    struct cst_tile tilemap[CST_TILEMAP_MAX_HEIGHT][CST_TILEMAP_MAX_WIDTH];
};

enum cst_tile_type cst_get(struct cst_tile_map *tilemap, int x, int y);

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type tile_type);

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos);

void cst_game_pos(struct em_ipos *pos, struct em_ipos *game_pos);

void cst_game_pos_2x(struct em_ipos_2x *pos, struct em_ipos_2x *game_pos);
