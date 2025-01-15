#pragma once

#include <caresto/opengl/coo_opengl.h>
#include <caresto/system/csc_camera.h>
#include <engine/eo_opengl.h>

#define CST_TILE_MAP_MAX_WIDTH 100
#define CST_TILE_MAP_MAX_HEIGHT 100
#define CST_TILES_MAX 1000

enum cst_tile_type : unsigned char {
    CST_EMPTY,
    CST_SOLID,
};

struct cst_tile_map {
    int tile_count;
    enum cst_tile_type tile_map[CST_TILE_MAP_MAX_HEIGHT]
                               [CST_TILE_MAP_MAX_WIDTH];
    struct coo_sprite tiles_gpu[CST_TILES_MAX];
};

void cst_to_coo_buffer(struct coo_sprite *sprites,
                       struct cst_tile_map *tilemap);

enum cst_tile_type cst_get(struct cst_tile_map *tilemap, int x, int y);

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type tile_type);

void cst_redraw(struct cst_tile_map *tile_map);

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos);

void cst_game_pos(struct em_ipos *pos, struct em_ipos *game_pos);

void cst_game_pos_2x(struct em_ipos_2x *pos, struct em_ipos_2x *game_pos);
