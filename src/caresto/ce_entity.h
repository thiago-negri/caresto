#ifndef CE_ENTITY_H
#define CE_ENTITY_H

#include <caresto/cb_bodymap.h>
#include <caresto/cs_spritemap.h>

struct ce_entity {
    cs_sprite_id sprite;
    cb_body_id body;
    struct eu_ivec2 position;
    struct eu_vec2 movement_remaining;
    struct eu_vec2 velocity;
};

void ce_tick(struct ce_entity *entity, struct cb_bodymap *bodymap,
             struct ct_tilemap *tilemap);

void ce_frame(struct ce_entity *entity, struct cs_spritemap *spritemap);

#endif // CE_ENTITY_H
