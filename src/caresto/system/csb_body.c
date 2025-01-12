#include <caresto/data/cdi_id.h>
#include <caresto/system/csb_body.h>
#include <engine/em_math.h>
#include <engine/et_test.h>
#include <gen/tile_atlas.h>
#include <string.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

typedef unsigned char csb_body_index;

CDI_IDS(csb_body_map, csb, csb_body_index, ids, CSB_BODIES_MAX)

ET_TEST(csb_sizes) {
    ET_ASSERT((csb_body_index)CSB_BODIES_MAX == CSB_BODIES_MAX)
    ET_ASSERT((csb_body_id)CSB_BODIES_MAX == CSB_BODIES_MAX)
    ET_DONE
}

// AABB collision
static bool csb_collision(struct csb_body *a, struct csb_body *b) {
    int a_left = a->position.x;
    int b_right = b->position.x + b->size.w;
    if (a_left > b_right)
        return false;

    int a_right = a->position.x + a->size.w;
    int b_left = b->position.x;
    if (a_right < b_left)
        return false;

    int a_top = a->position.y;
    int b_bottom = b->position.y + b->size.h;
    if (a_top > b_bottom)
        return false;

    int a_bottom = a->position.y + a->size.h;
    int b_top = b->position.y;
    if (a_bottom < b_top)
        return false;

    return true;
}

csb_body_id csb_add(struct csb_body_map *bodymap, struct csb_body *body) {
    el_assert(bodymap->body_count < CSB_BODIES_MAX);

    csb_body_index index = bodymap->body_count;
    csb_body_id id = csb_ids_new(bodymap, index);

    memcpy(&bodymap->bodies[index], body, sizeof(struct csb_body));
    bodymap->body_count++;

    return id;
}

struct csb_body *csb_get(struct csb_body_map *bodymap, csb_body_id id) {
    csb_body_index index = csb_ids_get(bodymap, id);
    return &bodymap->bodies[index];
}

void csb_remove(struct csb_body_map *bodymap, csb_body_id id) {
    csb_body_index index = csb_ids_rm(bodymap, id);

    bodymap->body_count--;
    if (index < bodymap->body_count) {
        memcpy(&bodymap->bodies[index], &bodymap->bodies[bodymap->body_count],
               sizeof(struct csb_body));

        csb_ids_move(bodymap, index, bodymap->body_count);
    }
}

static bool csb_move_ix(struct csb_body_map *bodymap,
                        struct cst_tile_map *tilemap, csb_body_index index,
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

    struct em_ipos top_left;
    struct em_ipos bottom_right;
    cst_game_pos(&top_left, &new_body.position);
    cst_game_pos(&bottom_right, &(struct em_ipos){
                                    .x = new_body.position.x + body->size.w,
                                    .y = new_body.position.y + body->size.h,
                                });
    for (int y = top_left.y; y <= bottom_right.y; y++) {
        for (int x = top_left.x; x <= bottom_right.x; x++) {
            enum cst_tile_type type = cst_get(tilemap, x, y);
            if (type != CST_EMPTY) {
                return false;
            }
        }
    }

    for (csb_body_index i = 0; i < bodymap->body_count; i++) {
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
    csb_body_index index = csb_ids_get(bodymap, id);
    return csb_move_ix(bodymap, tilemap, index, movement);
}

static bool csb_grounded_ix(struct csb_body_map *bodymap,
                            struct cst_tile_map *tilemap,
                            csb_body_index index) {
    struct csb_body *body = &bodymap->bodies[index];
    struct csb_body new_body = {
        .position =
            {
                .x = body->position.x,
                .y = body->position.y + 1,
            },
        .size = body->size,
    };

    struct em_ipos_2x bottom_left_right;
    cst_game_pos_2x(&bottom_left_right,
                    &(struct em_ipos_2x){
                        .x1 = new_body.position.x,
                        .x2 = new_body.position.x + body->size.w,
                        .y = new_body.position.y + body->size.h + 1,
                    });
    for (int x = bottom_left_right.x1; x <= bottom_left_right.x2; x++) {
        enum cst_tile_type type = cst_get(tilemap, x, bottom_left_right.y);
        if (type != CST_EMPTY) {
            return true;
        }
    }

    for (csb_body_index i = 0; i < bodymap->body_count; i++) {
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
    csb_body_index index = csb_ids_get(bodymap, id);
    return csb_grounded_ix(bodymap, tilemap, index);
}

void csb_tick(struct csb_body_map *bodymap, struct cst_tile_map *tilemap) {
    for (csb_body_index i = 0; i < bodymap->body_count; i++) {
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
