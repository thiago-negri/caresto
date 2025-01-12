#pragma once

#include <caresto/system/css_sprite.h>
#include <caresto/system/cst_tile.h>
#include <engine/em_math.h>

#define CSB_BODIES_MAX 256

typedef size_t csb_body_id;

struct csb_body {
    struct em_ipos position;
    struct em_isize size;
    struct em_vec2 velocity;
    struct em_vec2 movement_remaining;
};

struct csb_body_map {
    size_t body_count;
    size_t ids[CSB_BODIES_MAX];
    struct csb_body bodies[CSB_BODIES_MAX];
};

csb_body_id csb_add(struct csb_body_map *body_map, struct csb_body *body);
struct csb_body *csb_get(struct csb_body_map *body_map, csb_body_id id);
void csb_remove(struct csb_body_map *body_map, csb_body_id id);
bool csb_grounded(struct csb_body_map *body_map, struct cst_tile_map *tilemap,
                  csb_body_id id);
void csb_tick(struct csb_body_map *body_map, struct cst_tile_map *tilemap);
