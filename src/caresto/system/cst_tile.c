#include <caresto/system/csc_camera.h>
#include <caresto/system/cst_tile.h>
#include <engine/el_log.h>
#include <engine/em_math.h>
#include <engine/eo_opengl.h>
#include <gen/tile_atlas.h>

static void cst_remove_tile(struct cst_tile_map *tilemap, int index) {
    el_assert(tilemap->tile_count > 0);

    tilemap->tile_count -= 1;
    if (index < tilemap->tile_count) {
        memcpy(&tilemap->tiles[index], &tilemap->tiles[tilemap->tile_count],
               sizeof(struct eo_tile));
    }
}

static void cst_set_tile(struct cst_tile_map *tilemap, size_t index,
                         struct em_ivec2 position, enum cst_tile_type type) {
    el_assert(index < CST_TILES_MAX);

    tilemap->tiles[index].x = position.x;
    tilemap->tiles[index].y = position.y;
    switch (type) {
    case CST_SOLID:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_GRASS_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_GRASS_V;
        break;
    default:
        el_assert(false);
    }
}

enum cst_tile_type cst_get(struct cst_tile_map *tilemap, int x, int y) {
    el_assert(x < CST_TILEMAP_MAX_WIDTH);
    el_assert(y < CST_TILEMAP_MAX_HEIGHT);
    struct cst_tile *tile = &tilemap->tilemap[y][x];
    return tile->type;
}

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type new_tile_type) {
    el_assert(x < CST_TILEMAP_MAX_WIDTH);
    el_assert(y < CST_TILEMAP_MAX_HEIGHT);

    struct cst_tile *tile = &tilemap->tilemap[y][x];
    if (tile->type != CST_EMPTY) {
        if (new_tile_type == CST_EMPTY) {
            cst_remove_tile(tilemap, tile->tiles_index);
            tile->tiles_index = 0;
        }
    } else {
        if (new_tile_type != CST_EMPTY) {
            cst_set_tile(tilemap, tilemap->tile_count,
                         (struct em_ivec2){.x = x, .y = y}, new_tile_type);
            tile->tiles_index = tilemap->tile_count;
            tilemap->tile_count++;
        }
    }

    tile->type = new_tile_type;
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
