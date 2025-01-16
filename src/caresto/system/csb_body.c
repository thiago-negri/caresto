#include <caresto/data/cdi_id.h>
#include <caresto/system/csb_body.h>
#include <caresto/system/cst_tile.h>
#include <engine/et_test.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

typedef unsigned char csb_body_index;

CDI_IDS(cds_body_map, csb, csb_body_index, ids, CDS_BODIES_MAX)

ET_TEST(csb_sizes) {
    ET_ASSERT((csb_body_index)CDS_BODIES_MAX == CDS_BODIES_MAX)
    ET_ASSERT((cds_body_id)CDS_BODIES_MAX == CDS_BODIES_MAX)
    ET_DONE
}

// AABB collision
static bool csb_collision(struct cds_body *a, struct cds_body *b) {
    int a_left = a->position.x;
    int b_right = b->position.x + b->size.w;
    if (a_left >= b_right)
        return false;

    int a_right = a->position.x + a->size.w;
    int b_left = b->position.x;
    if (a_right <= b_left)
        return false;

    int a_top = a->position.y;
    int b_bottom = b->position.y + b->size.h;
    if (a_top >= b_bottom)
        return false;

    int a_bottom = a->position.y + a->size.h;
    int b_top = b->position.y;
    if (a_bottom <= b_top)
        return false;

    return true;
}

cds_body_id csb_add(struct cds_systems *systems, struct cds_body *body) {
    el_assert(systems->body_map.body_count < CDS_BODIES_MAX);

    csb_body_index index = systems->body_map.body_count;
    cds_body_id id = csb_ids_new(&systems->body_map, index);

    memcpy(&systems->body_map.bodies[index], body, sizeof(struct cds_body));
    systems->body_map.body_count++;

    return id;
}

struct cds_body *csb_get(struct cds_systems *systems, cds_body_id id) {
    csb_body_index index = csb_ids_get(&systems->body_map, id);
    return &systems->body_map.bodies[index];
}

void csb_remove(struct cds_systems *systems, cds_body_id id) {
    csb_body_index index = csb_ids_rm(&systems->body_map, id);

    systems->body_map.body_count--;
    if (index < systems->body_map.body_count) {
        memcpy(&systems->body_map.bodies[index],
               &systems->body_map.bodies[systems->body_map.body_count],
               sizeof(struct cds_body));

        csb_ids_move(&systems->body_map, index, systems->body_map.body_count);
    }
}

static bool csb_try_move_ix(struct cds_systems *systems, csb_body_index index,
                            struct em_ivec2 *movement) {
    struct cds_body *body = &systems->body_map.bodies[index];
    struct cds_body new_body = {
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
                                    .x = new_body.position.x + body->size.w - 1,
                                    .y = new_body.position.y + body->size.h - 1,
                                });
    for (int y = top_left.y; y <= bottom_right.y; y++) {
        for (int x = top_left.x; x <= bottom_right.x; x++) {
            enum cds_tile_type type = cst_get(&systems->tile_map, x, y);
            if (type != CDS_EMPTY) {
                return false;
            }
        }
    }

    for (csb_body_index i = 0; i < systems->body_map.body_count; i++) {
        if (i == index) {
            continue;
        }
        struct cds_body *other = &systems->body_map.bodies[i];
        if (csb_collision(&new_body, other)) {
            return false;
        }
    }

    body->position.x = new_body.position.x;
    body->position.y = new_body.position.y;
    return true;
}

static bool csb_move_ix(struct cds_systems *systems, csb_body_index index,
                        struct em_ivec2 *movement) {
    int x_remaining = movement->x;
    int x_move = em_sign(x_remaining);

    int y_remaining = movement->y;
    int y_move = em_sign(y_remaining);

    bool moved_at_all = false;

    while (x_remaining != 0) {
        bool moved = csb_try_move_ix(systems, index,
                                     &(struct em_ivec2){.x = x_move, .y = 0});
        if (!moved) {
            x_remaining = 0;
        } else {
            x_remaining -= x_move;
            moved_at_all = true;
        }
    }

    while (y_remaining != 0) {
        bool moved = csb_try_move_ix(systems, index,
                                     &(struct em_ivec2){.x = 0, .y = y_move});
        if (!moved) {
            y_remaining = 0;
        } else {
            y_remaining -= y_move;
            moved_at_all = true;
        }
    }

    return moved_at_all;
}

bool csb_move(struct cds_systems *systems, cds_body_id id,
              struct em_ivec2 *movement) {
    csb_body_index index = csb_ids_get(&systems->body_map, id);
    return csb_move_ix(systems, index, movement);
}

static bool csb_grounded_ix(struct cds_systems *systems, csb_body_index index) {
    struct cds_body *body = &systems->body_map.bodies[index];
    struct cds_body new_body = {
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
                        .x2 = new_body.position.x + body->size.w - 1,
                        .y = new_body.position.y + body->size.h - 1,
                    });
    for (int x = bottom_left_right.x1; x <= bottom_left_right.x2; x++) {
        enum cds_tile_type type =
            cst_get(&systems->tile_map, x, bottom_left_right.y);
        if (type != CDS_EMPTY) {
            return true;
        }
    }

    for (csb_body_index i = 0; i < systems->body_map.body_count; i++) {
        if (i == index) {
            continue;
        }
        struct cds_body *other = &systems->body_map.bodies[i];
        if (csb_collision(&new_body, other)) {
            return true;
        }
    }

    return false;
}

bool csb_grounded(struct cds_systems *systems, cds_body_id id) {
    csb_body_index index = csb_ids_get(&systems->body_map, id);
    return csb_grounded_ix(systems, index);
}

void csb_tick(struct cds_systems *systems) {
    for (csb_body_index i = 0; i < systems->body_map.body_count; i++) {
        struct cds_body *body = &systems->body_map.bodies[i];
        bool grounded = csb_grounded_ix(systems, i);
        if (grounded && body->velocity.y >= 0) {
            body->velocity.y = 0;
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
            csb_move_ix(systems, i, &movement);
        }

        body->movement_remaining.x -= movement.x;
        body->movement_remaining.y -= movement.y;
    }
}
