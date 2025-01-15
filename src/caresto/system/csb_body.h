#pragma once

#include <caresto/system/csd_debug.h>
#include <caresto/system/css_sprite.h>
#include <caresto/system/cst_tile.h>
#include <engine/em_math.h>

#define CSB_BODIES_MAX 100

typedef unsigned char csb_body_id;

struct csb_body {
    struct em_ipos position;
    struct em_isize size;
    struct em_vec2 velocity;
    struct em_vec2 movement_remaining;
};

struct csb_body_map {
    unsigned char body_count;
    unsigned char ids[CSB_BODIES_MAX];
    struct csb_body bodies[CSB_BODIES_MAX];
};

csb_body_id csb_add(struct csb_body_map *body_map, struct csb_body *body);
struct csb_body *csb_get(struct csb_body_map *body_map, csb_body_id id);
void csb_remove(struct csb_body_map *body_map, csb_body_id id);
bool csb_grounded(struct csb_body_map *body_map, struct cst_tile_map *tilemap,
                  csb_body_id id);
void csb_tick(struct csb_body_map *body_map, struct cst_tile_map *tilemap,
              struct csd_debug *debug);
