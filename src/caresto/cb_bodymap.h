#ifndef CB_BODYMAP_H
#define CB_BODYMAP_H

#include <stdbool.h>

#include <engine/eu_utils.h>

#include <caresto/ct_tilemap.h>

#define CB_BODIES_MAX 1024

typedef size_t body_id;

struct cb_body {
    struct eu_ivec2 position;
    struct eu_ivec2 size;
};

struct cb_bodymap {
    size_t body_count;
    struct cb_body bodies[CB_BODIES_MAX];
};

body_id cb_add(struct cb_bodymap *bodymap, struct cb_body *body);
struct cb_body *cb_get(struct cb_bodymap *bodymap, body_id id);
void cb_remove(struct cb_bodymap *bodymap, body_id id);
bool cb_move(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap, body_id id,
             struct eu_ivec2 *movement);

#endif // CB_BODYMAP_H
