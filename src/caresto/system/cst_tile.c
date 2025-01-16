#include <caresto/data/cdi_id.h>
#include <caresto/system/csc_camera.h>
#include <caresto/system/cst_tile.h>
#include <gen/tile_atlas.h>

enum cds_tile_type cst_get(struct cds_tile_map *tile_map, int x, int y) {
    el_assert(x < CDS_TILE_MAP_MAX_WIDTH);
    el_assert(y < CDS_TILE_MAP_MAX_HEIGHT);
    return tile_map->tile_map[y][x];
}

void cst_redraw(struct cds_tile_map *tile_map) {
    int count = 0;

    // Template
    struct coo_sprite sprite = {
        .vertex =
            {
                // Top left
                [0] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U,
                                   .v = GEN_TILE_ATLAS_GRASS_V}},
                // Bottom left
                [1] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U,
                                   .v = GEN_TILE_ATLAS_GRASS_V +
                                        GEN_TILE_ATLAS_TILE_SIZE}},
                // Top right
                [2] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U +
                                        GEN_TILE_ATLAS_TILE_SIZE,
                                   .v = GEN_TILE_ATLAS_GRASS_V}},
                // Top right
                [3] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U +
                                        GEN_TILE_ATLAS_TILE_SIZE,
                                   .v = GEN_TILE_ATLAS_GRASS_V}},
                // Bottom left
                [4] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U,
                                   .v = GEN_TILE_ATLAS_GRASS_V +
                                        GEN_TILE_ATLAS_TILE_SIZE}},
                // Bottom right
                [5] = {.position = {},
                       .texture = {.u = GEN_TILE_ATLAS_GRASS_U +
                                        GEN_TILE_ATLAS_TILE_SIZE,
                                   .v = GEN_TILE_ATLAS_GRASS_V +
                                        GEN_TILE_ATLAS_TILE_SIZE}},
            },
    };

    for (int y = 0; y < CDS_TILE_MAP_MAX_HEIGHT; y++) {
        int y_offset = y * GEN_TILE_ATLAS_TILE_SIZE;
        for (int x = 0; x < CDS_TILE_MAP_MAX_WIDTH; x++) {
            enum cds_tile_type tile = tile_map->tile_map[y][x];

            switch (tile) {
            case CDS_EMPTY:
                break;

            case CDS_SOLID:
                int x_offset = x * GEN_TILE_ATLAS_TILE_SIZE;

                int top = y_offset;
                int bottom = y_offset + GEN_TILE_ATLAS_TILE_SIZE;
                int left = x_offset;
                int right = x_offset + GEN_TILE_ATLAS_TILE_SIZE;

                // Top left
                sprite.vertex[0].position.y = top;
                sprite.vertex[0].position.x = left;

                // Bottom left
                sprite.vertex[1].position.y = bottom;
                sprite.vertex[1].position.x = left;

                // Top right
                sprite.vertex[2].position.y = top;
                sprite.vertex[2].position.x = right;

                // Top right
                sprite.vertex[3].position.y = top;
                sprite.vertex[3].position.x = right;

                // Bottom left
                sprite.vertex[4].position.y = bottom;
                sprite.vertex[4].position.x = left;

                // Bottom right
                sprite.vertex[5].position.y = bottom;
                sprite.vertex[5].position.x = right;

                memcpy(&tile_map->tiles_gpu[count], &sprite,
                       sizeof(struct coo_sprite));

                count++;
                break;
            }
        }
    }

    tile_map->tile_count = count;
}

void cst_set(struct cds_tile_map *tile_map, int x, int y,
             enum cds_tile_type new_tile_type) {
    el_assert(x < CDS_TILE_MAP_MAX_WIDTH);
    el_assert(y < CDS_TILE_MAP_MAX_HEIGHT);
    tile_map->tile_map[y][x] = new_tile_type;
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
