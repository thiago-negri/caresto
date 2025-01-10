#ifndef CE_ENTITY_H
#define CE_ENTITY_H

#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/cs_spritemap.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>

struct ce_entity {
    cs_sprite_id sprite;
    cb_body_id body;
    ca_animation_id animation;
    enum gen_animation_index animation_idle;
    enum gen_animation_index animation_walk;
    struct eu_ivec2 position;
    struct eu_vec2 movement_remaining;
    struct eu_vec2 velocity;
};

void ce_tick(struct ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             struct cs_spritemap *spritemap);

void ce_frame(struct ce_entity *entity, struct cs_spritemap *spritemap);

#endif // CE_ENTITY_H
