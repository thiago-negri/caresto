#include <caresto/system/csc_camera.h>
#include <caresto/system/cst_tile.h>
#include <engine/el_log.h>
#include <engine/em_math.h>
#include <engine/eo_opengl.h>
#include <gen/tile_atlas.h>

enum cst_tile_type cst_get(struct cst_tile_map *tilemap, int x, int y) {
    el_assert(x < CST_TILEMAP_MAX_WIDTH);
    el_assert(y < CST_TILEMAP_MAX_HEIGHT);
    return tilemap->tilemap[y][x];
}

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type new_tile_type) {
    el_assert(x < CST_TILEMAP_MAX_WIDTH);
    el_assert(y < CST_TILEMAP_MAX_HEIGHT);

    enum cst_tile_type *tile = &tilemap->tilemap[y][x];
    if (*tile != CST_EMPTY) {
        if (new_tile_type == CST_EMPTY) {
            tilemap->tile_count--;
        }
    } else {
        if (new_tile_type != CST_EMPTY) {
            tilemap->tile_count++;
        }
    }

    *tile = new_tile_type;
}

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos) {
    float world_x =
        em_lerp(cam_bounds->left, cam_bounds->right, win_pos->x / win_size->w);
    float world_y =
        em_lerp(cam_bounds->top, cam_bounds->bottom, win_pos->y / win_size->h);
    el_assert(world_x >= 0);
    el_assert(world_y >= 0);
    pos->x = world_x / GEN_TILE_ATLAS_TILE_SIZE;
    pos->y = world_y / GEN_TILE_ATLAS_TILE_SIZE;
}

void cst_game_pos(struct em_ipos *pos, struct em_ipos *game_pos) {
    pos->x = game_pos->x / GEN_TILE_ATLAS_TILE_SIZE;
    pos->y = game_pos->y / GEN_TILE_ATLAS_TILE_SIZE;
}

void cst_game_pos_2x(struct em_ipos_2x *pos, struct em_ipos_2x *game_pos) {
    pos->x1 = game_pos->x1 / GEN_TILE_ATLAS_TILE_SIZE;
    pos->x2 = game_pos->x2 / GEN_TILE_ATLAS_TILE_SIZE;
    pos->y = game_pos->y / GEN_TILE_ATLAS_TILE_SIZE;
}

void cst_to_coo_buffer(struct coo_sprite *sprites,
                       struct cst_tile_map *tilemap) {
    // TODO(tnegri): convert from tilemap into sprites
}
