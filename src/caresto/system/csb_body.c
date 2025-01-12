#include <caresto/data/cdi_id.h>
#include <caresto/system/csb_body.h>
#include <engine/em_math.h>
#include <gen/tile_atlas.h>
#include <string.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

CDI_IDS(csb_body_map, csb, ids, CSB_BODIES_MAX)

static bool csb_collision(struct csb_body *a, struct csb_body *b) {
    int a_left = a->position.x;
    int a_right = a->position.x + a->size.w;
    int a_top = a->position.y;
    int a_bottom = a->position.y + a->size.h;

    int b_left = b->position.x;
    int b_right = b->position.x + b->size.w;
    int b_top = b->position.y;
    int b_bottom = b->position.y + b->size.h;

    return (a_left <= b_right && a_right >= b_left && a_top <= b_bottom &&
            a_bottom >= b_top);
}

csb_body_id csb_add(struct csb_body_map *bodymap, struct csb_body *body) {
    el_assert(bodymap->body_count < CSB_BODIES_MAX);

    size_t index = bodymap->body_count;
    csb_body_id id = csb_ids_new(bodymap, index);

    memcpy(&bodymap->bodies[index], body, sizeof(struct csb_body));
    bodymap->body_count++;

    return id;
}

struct csb_body *csb_get(struct csb_body_map *bodymap, csb_body_id id) {
    size_t index = csb_ids_get(bodymap, id);
    return &bodymap->bodies[index];
}

void csb_remove(struct csb_body_map *bodymap, csb_body_id id) {
    size_t index = csb_ids_rm(bodymap, id);

    bodymap->body_count--;
    if (bodymap->body_count > 0) {
        memcpy(&bodymap->bodies[index], &bodymap->bodies[bodymap->body_count],
               sizeof(struct csb_body));

        csb_ids_move(bodymap, index, bodymap->body_count);
    }
}

static bool csb_move_ix(struct csb_body_map *bodymap,
                        struct cst_tile_map *tilemap, size_t index,
                        struct em_ivec2 *movement) {
    struct csb_body *body = &bodymap->bodies[index];
    struct csb_body new_body = {
        .position =
            {
                .x = body->position.x + movement->x,
                .y = body->position.y + movement->y,
            },
        .size = body->size,
    };

    struct em_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                 GEN_TILE_ATLAS_TILE_SIZE};
    struct em_ixpos top_left;
    struct em_ixpos bottom_right;
    cst_game_pos(&top_left, &new_body.position, &tile_size);
    cst_game_pos(&bottom_right,
                 &(struct em_ipos){
                     .x = new_body.position.x + body->size.w,
                     .y = new_body.position.y + body->size.h,
                 },
                 &tile_size);
    for (size_t y = top_left.y; y <= bottom_right.y; y++) {
        for (size_t x = top_left.x; x <= bottom_right.x; x++) {
            if (tilemap->tilemap[y][x].type != CST_TILE_TYPE_EMPTY) {
                return false;
            }
        }
    }

    for (size_t i = 0; i < bodymap->body_count; i++) {
        if (i == index) {
            continue;
        }
        struct csb_body *other = &bodymap->bodies[i];
        if (csb_collision(&new_body, other)) {
            return false;
        }
    }

    body->position.x = new_body.position.x;
    body->position.y = new_body.position.y;
    return true;
}

bool csb_move(struct csb_body_map *bodymap, struct cst_tile_map *tilemap,
              csb_body_id id, struct em_ivec2 *movement) {
    size_t index = csb_ids_get(bodymap, id);
    return csb_move_ix(bodymap, tilemap, index, movement);
}

static bool csb_grounded_ix(struct csb_body_map *bodymap,
                            struct cst_tile_map *tilemap, size_t index) {
    struct csb_body *body = &bodymap->bodies[index];
    struct csb_body new_body = {
        .position =
            {
                .x = body->position.x,
                .y = body->position.y + 1,
            },
        .size = body->size,
    };

    struct em_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                 GEN_TILE_ATLAS_TILE_SIZE};
    struct em_ixpos_2x bottom_left_right;
    cst_game_pos_2x(&bottom_left_right,
                    &(struct em_ipos_2x){
                        .x1 = new_body.position.x,
                        .x2 = new_body.position.x + body->size.w,
                        .y = new_body.position.y + body->size.h + 1,
                    },
                    &tile_size);
    for (size_t x = bottom_left_right.x1; x <= bottom_left_right.x2; x++) {
        struct cst_tile tile = tilemap->tilemap[bottom_left_right.y][x];
        if (tile.type != CST_TILE_TYPE_EMPTY) {
            return true;
        }
    }

    for (size_t i = 0; i < bodymap->body_count; i++) {
        if (i == index) {
            continue;
        }
        struct csb_body *other = &bodymap->bodies[i];
        if (csb_collision(&new_body, other)) {
            return true;
        }
    }

    return false;
}

bool csb_grounded(struct csb_body_map *bodymap, struct cst_tile_map *tilemap,
                  csb_body_id id) {
    size_t index = csb_ids_get(bodymap, id);
    return csb_grounded_ix(bodymap, tilemap, index);
}

void csb_tick(struct csb_body_map *bodymap, struct cst_tile_map *tilemap) {
    for (size_t i = 0; i < bodymap->body_count; i++) {
        struct csb_body *body = &bodymap->bodies[i];
        bool grounded = csb_grounded_ix(bodymap, tilemap, i);
        if (grounded) {
            if (body->velocity.y >= 0) {
                body->velocity.y = 0;
            }
        } else {
            body->velocity.y += GRAVITY_ACCELERATION_PER_TICK;
            if (body->velocity.y > GRAVITY_MAX_VELOCITY) {
                body->velocity.y = GRAVITY_MAX_VELOCITY;
            }
        }

        body->movement_remaining.x += body->velocity.x;
        body->movement_remaining.y += body->velocity.y;

        struct em_ivec2 movement = {
            .x = (int)body->movement_remaining.x,
            .y = (int)body->movement_remaining.y,
        };

        if (movement.x != 0 || movement.y != 0) {
            bool moved = csb_move_ix(bodymap, tilemap, i, &movement);
            if (!moved) {
                body->velocity.y = 0.0f;
            }
        }

        body->movement_remaining.x -= movement.x;
        body->movement_remaining.y -= movement.y;
    }
}
