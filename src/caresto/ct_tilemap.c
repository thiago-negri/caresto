#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/eu_utils.h>

#include <caresto/ct_tilemap.h>

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

void ct_set(struct ct_tilemap *tilemap, size_t x, size_t y,
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

void ct_tile_pos(GLfloat screen_left, GLfloat screen_right, GLfloat screen_top,
                 GLfloat screen_bottom, GLfloat screen_w, GLfloat screen_h,
                 GLfloat screen_x, GLfloat screen_y, int tile_size_w,
                 int tile_size_h, size_t *out_tile_x, size_t *out_tile_y) {
    float world_x = eu_lerp(screen_left, screen_right, screen_x / screen_w);
    float world_y = eu_lerp(screen_top, screen_bottom, screen_y / screen_h);
    *out_tile_x = world_x / tile_size_w;
    *out_tile_y = world_y / tile_size_h;
}
