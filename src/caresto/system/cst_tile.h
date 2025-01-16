#pragma once

#include <caresto/data/cds_systems.h>
#include <caresto/system/csc_camera.h>

void cst_to_coo_buffer(struct coo_sprite *sprites,
                       struct cds_tile_map *tilemap);

enum cds_tile_type cst_get(struct cds_tile_map *tilemap, int x, int y);

void cst_set(struct cds_tile_map *tilemap, int x, int y,
             enum cds_tile_type tile_type);

void cst_redraw(struct cds_tile_map *tile_map);

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos);

void cst_game_pos(struct em_ipos *pos, struct em_ipos *game_pos);

void cst_game_pos_2x(struct em_ipos_2x *pos, struct em_ipos_2x *game_pos);
