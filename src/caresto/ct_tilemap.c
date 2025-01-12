#include <caresto/cc_camera.h>
#include <caresto/ct_tilemap.h>
#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/eu_utils.h>
#include <gen/tile_atlas.h>

static void ct_remove_tile(struct ct_tilemap *tilemap, size_t index) {
    eu_assert(tilemap->tile_count > 0);

    tilemap->tile_count -= 1;
    if (tilemap->tile_count > 0) {
        tilemap->tiles[index] = tilemap->tiles[tilemap->tile_count];
    }
}

static void ct_set_tile(struct ct_tilemap *tilemap, size_t index,
                        struct eu_ivec2 position, enum ct_tile_type type) {
    eu_assert(index < CT_TILES_MAX);

    tilemap->tiles[index].x = position.x;
    tilemap->tiles[index].y = position.y;
    switch (type) {
    case CT_TILE_TYPE_GRASS:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_GRASS_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_GRASS_V;
        break;
    case CT_TILE_TYPE_SAND:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_SAND_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_SAND_V;
        break;
    case CT_TILE_TYPE_WATER:
        tilemap->tiles[index].u = GEN_TILE_ATLAS_WATER_U;
        tilemap->tiles[index].v = GEN_TILE_ATLAS_WATER_V;
        break;
    default:
        eu_assert(false);
    }
}

void ct_set(struct ct_tilemap *tilemap, int x, int y,
            enum ct_tile_type new_tile_type) {
    eu_assert(x < CT_TILEMAP_MAX_WIDTH);
    eu_assert(y < CT_TILEMAP_MAX_HEIGHT);

    struct ct_tile *tile = &tilemap->tilemap[y][x];
    if (tile->type != CT_TILE_TYPE_EMPTY) {
        if (new_tile_type == CT_TILE_TYPE_EMPTY) {
            ct_remove_tile(tilemap, tile->tiles_index);
            tile->tiles_index = 0;
        }
    } else {
        if (new_tile_type != CT_TILE_TYPE_EMPTY) {
            ct_set_tile(tilemap, tilemap->tile_count,
                        (struct eu_ivec2){.x = x, .y = y}, new_tile_type);
            tile->tiles_index = tilemap->tile_count;
            tilemap->tile_count++;
        }
    }

    tile->type = new_tile_type;
}

void ct_screen_pos(struct eu_ipos *pos, struct cc_bounds *cam_bounds,
                   struct eu_isize *win_size, struct eu_fpos *win_pos,
                   struct eu_isize *tile_size) {
    float world_x =
        eu_lerp(cam_bounds->left, cam_bounds->right, win_pos->x / win_size->w);
    float world_y =
        eu_lerp(cam_bounds->top, cam_bounds->bottom, win_pos->y / win_size->h);
    eu_assert(world_x >= 0);
    eu_assert(world_y >= 0);
    pos->x = world_x / tile_size->w;
    pos->y = world_y / tile_size->h;
}

void ct_game_pos(struct eu_ixpos *pos, struct eu_ipos *game_pos,
                 struct eu_isize *tile_size) {
    pos->x = game_pos->x / tile_size->w;
    pos->y = game_pos->y / tile_size->h;
}

void ct_game_pos_2x(struct eu_ixpos_2x *pos, struct eu_ipos_2x *game_pos,
                    struct eu_isize *tile_size) {
    pos->x1 = game_pos->x1 / tile_size->w;
    pos->x2 = game_pos->x2 / tile_size->w;
    pos->y = game_pos->y / tile_size->h;
}
