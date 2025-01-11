#include <caresto/cb_bodymap.h>
#include <caresto/ci_ids.h>
#include <engine/eu_utils.h>
#include <gen/tile_atlas.h>
#include <string.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

CI_IDS(cb_bodymap, cb, ids, CB_BODIES_MAX)

static bool cb_collision(struct cb_body *a, struct cb_body *b) {
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

cb_body_id cb_add(struct cb_bodymap *bodymap, struct cb_body *body) {
    eu_assert(bodymap->body_count < CB_BODIES_MAX);

    size_t index = bodymap->body_count;
    cb_body_id id = cb_ids_new(bodymap, index);

    memcpy(&bodymap->bodies[index], body, sizeof(struct cb_body));
    bodymap->body_count++;

    return id;
}

struct cb_body *cb_get(struct cb_bodymap *bodymap, cb_body_id id) {
    size_t index = cb_ids_get(bodymap, id);
    return &bodymap->bodies[index];
}

void cb_remove(struct cb_bodymap *bodymap, cb_body_id id) {
    size_t index = cb_ids_rm(bodymap, id);

    bodymap->body_count--;
    if (bodymap->body_count > 0) {
        memcpy(&bodymap->bodies[index], &bodymap->bodies[bodymap->body_count],
               sizeof(struct cb_body));

        cb_ids_move(bodymap, index, bodymap->body_count);
    }
}

static bool cb_move_ix(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
                       size_t index, struct eu_ivec2 *movement) {
    struct cb_body *body = &bodymap->bodies[index];
    struct cb_body new_body = {
        .position =
            {
                .x = body->position.x + movement->x,
                .y = body->position.y + movement->y,
            },
        .size = body->size,
    };

    struct eu_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                 GEN_TILE_ATLAS_TILE_SIZE};
    struct eu_ixpos top_left;
    struct eu_ixpos bottom_right;
    ct_game_pos(&top_left, &new_body.position, &tile_size);
    ct_game_pos(&bottom_right,
                &(struct eu_ipos){
                    .x = new_body.position.x + body->size.w,
                    .y = new_body.position.y + body->size.h,
                },
                &tile_size);
    for (size_t y = top_left.y; y <= bottom_right.y; y++) {
        for (size_t x = top_left.x; x <= bottom_right.x; x++) {
            if (tilemap->tilemap[y][x].type != CT_TILE_TYPE_EMPTY) {
                return false;
            }
        }
    }

    for (size_t i = 0; i < bodymap->body_count; i++) {
        if (i == index) {
            continue;
        }
        struct cb_body *other = &bodymap->bodies[i];
        if (cb_collision(&new_body, other)) {
            return false;
        }
    }

    body->position.x = new_body.position.x;
    body->position.y = new_body.position.y;
    return true;
}

bool cb_move(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             cb_body_id id, struct eu_ivec2 *movement) {
    size_t index = cb_ids_get(bodymap, id);
    return cb_move_ix(bodymap, tilemap, index, movement);
}

static bool cb_grounded_ix(struct cb_bodymap *bodymap,
                           struct ct_tilemap *tilemap, size_t index) {
    struct cb_body *body = &bodymap->bodies[index];
    struct cb_body new_body = {
        .position =
            {
                .x = body->position.x,
                .y = body->position.y + 1,
            },
        .size = body->size,
    };

    struct eu_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                 GEN_TILE_ATLAS_TILE_SIZE};
    struct eu_ixpos_2x bottom_left_right;
    ct_game_pos_2x(&bottom_left_right,
                   &(struct eu_ipos_2x){
                       .x1 = new_body.position.x,
                       .x2 = new_body.position.x + body->size.w,
                       .y = new_body.position.y + body->size.h + 1,
                   },
                   &tile_size);
    for (size_t x = bottom_left_right.x1; x <= bottom_left_right.x2; x++) {
        struct ct_tile tile = tilemap->tilemap[bottom_left_right.y][x];
        if (tile.type != CT_TILE_TYPE_EMPTY) {
            return true;
        }
    }

    for (size_t i = 0; i < bodymap->body_count; i++) {
        if (i == index) {
            continue;
        }
        struct cb_body *other = &bodymap->bodies[i];
        if (cb_collision(&new_body, other)) {
            return true;
        }
    }

    return false;
}

bool cb_grounded(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
                 cb_body_id id) {
    size_t index = cb_ids_get(bodymap, id);
    return cb_grounded_ix(bodymap, tilemap, index);
}

void cb_tick(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap) {
    for (size_t i = 0; i < bodymap->body_count; i++) {
        struct cb_body *body = &bodymap->bodies[i];
        bool grounded = cb_grounded_ix(bodymap, tilemap, i);
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

        struct eu_ivec2 movement = {
            .x = (int)body->movement_remaining.x,
            .y = (int)body->movement_remaining.y,
        };

        if (movement.x != 0 || movement.y != 0) {
            bool moved = cb_move_ix(bodymap, tilemap, i, &movement);
            if (!moved) {
                body->velocity.y = 0.0f;
            }
        }

        body->movement_remaining.x -= movement.x;
        body->movement_remaining.y -= movement.y;
    }
}
