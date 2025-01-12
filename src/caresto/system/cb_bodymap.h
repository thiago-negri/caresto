#ifndef CB_BODYMAP_H
#define CB_BODYMAP_H

#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>
#include <engine/eu_utils.h>

#define CB_BODIES_MAX 256

typedef size_t cb_body_id;

struct cb_body {
    struct eu_ipos position;
    struct eu_isize size;
    struct eu_vec2 velocity;
    struct eu_vec2 movement_remaining;
};

struct cb_bodymap {
    size_t body_count;
    size_t ids[CB_BODIES_MAX];
    struct cb_body bodies[CB_BODIES_MAX];
};

cb_body_id cb_add(struct cb_bodymap *bodymap, struct cb_body *body);
struct cb_body *cb_get(struct cb_bodymap *bodymap, cb_body_id id);
void cb_remove(struct cb_bodymap *bodymap, cb_body_id id);
bool cb_grounded(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
                 cb_body_id id);
void cb_tick(struct cb_bodymap *bodymap, struct ct_tilemap *tilemap);

#endif // CB_BODYMAP_H
