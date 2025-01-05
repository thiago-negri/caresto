#include <string.h>

#include <engine/eu_utils.h>

#include <caresto/cb_bodymap.h>

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

body_id cb_add(struct cb_bodymap *bodymap, struct cb_body *body) {
    eu_assert(bodymap->body_count < CB_BODIES_MAX);

    body_id id = bodymap->body_count;
    memcpy(&bodymap->bodies[id], body, sizeof(struct cb_body));
    bodymap->body_count++;
    return id;
}

struct cb_body *cb_get(struct cb_bodymap *bodymap, body_id id) {
    eu_assert(id < bodymap->body_count);

    return &bodymap->bodies[id];
}

void cb_remove(struct cb_bodymap *bodymap, body_id id) {
    eu_assert(id < bodymap->body_count);

    bodymap->body_count--;
    if (bodymap->body_count > 0) {
        memcpy(&bodymap->bodies[id], &bodymap->bodies[bodymap->body_count],
               sizeof(struct cb_body));
    }
}

bool cb_move(struct cb_bodymap *bodymap, body_id id,
             struct eu_ivec2 *movement) {
    eu_assert(id < bodymap->body_count);

    struct cb_body *body = &bodymap->bodies[id];
    struct cb_body new_body = {
        .position =
            {
                .x = body->position.x + movement->x,
                .y = body->position.y + movement->y,
            },
        .size = body->size,
    };

    for (size_t i = 0; i < bodymap->body_count; i++) {
        if (i == id) {
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
