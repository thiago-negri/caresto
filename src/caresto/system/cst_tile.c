#include <caresto/system/csc_camera.h>
#include <caresto/system/cst_tile.h>
#include <engine/el_log.h>
#include <engine/em_math.h>
#include <engine/eo_opengl.h>
#include <gen/tile_atlas.h>

static void cst_remove_tile(struct cst_tile_map *tilemap, size_t index) {
    el_assert(tilemap->tile_count > 0);

    tilemap->tile_count -= 1;
    if (tilemap->tile_count > 0) {
        tilemap->tiles[index] = tilemap->tiles[tilemap->tile_count];
    }
}

static void cst_set_tile(struct cst_tile_map *tilemap, size_t index,
                         struct em_ivec2 position, enum cst_tile_type type) {
    el_assert(index < CST_TILES_MAX);

    tilemap->tiles[index].x = position.x;
    tilemap->tiles[index].y = position.y;
    switch (type) {
    case CST_TILE_TYPE_GRASS:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_GRASS_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_GRASS_V;
        break;
    case CST_TILE_TYPE_SAND:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_SAND_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_SAND_V;
        break;
    case CST_TILE_TYPE_WATER:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_WATER_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_WATER_V;
        break;
    default:
        el_assert(false);
    }
}

void cst_set(struct cst_tile_map *tilemap, int x, int y,
             enum cst_tile_type new_tile_type) {
    el_assert(x < CST_TILEMAP_MAX_WIDTH);
    el_assert(y < CST_TILEMAP_MAX_HEIGHT);

    struct cst_tile *tile = &tilemap->tilemap[y][x];
    if (tile->type != CST_TILE_TYPE_EMPTY) {
        if (new_tile_type == CST_TILE_TYPE_EMPTY) {
            cst_remove_tile(tilemap, tile->tiles_index);
            tile->tiles_index = 0;
        }
    } else {
        if (new_tile_type != CST_TILE_TYPE_EMPTY) {
            cst_set_tile(tilemap, tilemap->tile_count,
                         (struct em_ivec2){.x = x, .y = y}, new_tile_type);
            tile->tiles_index = tilemap->tile_count;
            tilemap->tile_count++;
        }
    }

    tile->type = new_tile_type;
}

void cst_screen_pos(struct em_ipos *pos, struct csc_bounds *cam_bounds,
                    struct em_isize *win_size, struct em_fpos *win_pos,
                    struct em_isize *tile_size) {
    float world_x =
        em_lerp(cam_bounds->left, cam_bounds->right, win_pos->x / win_size->w);
    float world_y =
        em_lerp(cam_bounds->top, cam_bounds->bottom, win_pos->y / win_size->h);
    el_assert(world_x >= 0);
    el_assert(world_y >= 0);
    pos->x = world_x / tile_size->w;
    pos->y = world_y / tile_size->h;
}

void cst_game_pos(struct em_ixpos *pos, struct em_ipos *game_pos,
                  struct em_isize *tile_size) {
    pos->x = game_pos->x / tile_size->w;
    pos->y = game_pos->y / tile_size->h;
}

void cst_game_pos_2x(struct em_ixpos_2x *pos, struct em_ipos_2x *game_pos,
                     struct em_isize *tile_size) {
    pos->x1 = game_pos->x1 / tile_size->w;
    pos->x2 = game_pos->x2 / tile_size->w;
    pos->y = game_pos->y / tile_size->h;
}
