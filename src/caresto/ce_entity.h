#ifndef CE_ENTITY_H
#define CE_ENTITY_H

#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/cs_spritemap.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>

/*enum ce_type {*/
/*    ce_beetle,*/
/*    ce_carestosan,*/
/*};*/
/**/
/*struct ce_beetle {*/
/*    enum ce_type type;*/
/*    cs_sprite_id sprite;*/
/*    cb_body_id body;*/
/*    ca_animation_id animation;*/
/*    struct eu_ivec2 position;*/
/*    struct eu_vec2 movement_remaining;*/
/*    struct eu_vec2 velocity;*/
/*};*/
/**/
/*struct ce_carestosan {*/
/*    enum ce_type type;*/
/*    cs_sprite_id sprite;*/
/*    cb_body_id body;*/
/*    ca_animation_id animation;*/
/*    struct eu_ivec2 position;*/
/*    struct eu_vec2 movement_remaining;*/
/*    struct eu_vec2 velocity;*/
/*};*/
/**/
/*union ce_entity {*/
/*    enum ce_type type;*/
/*    struct ce_beetle beetle;*/
/*    struct ce_carestosan carestosan;*/
/*};*/

struct ce_entity {
    cs_sprite_id sprite;
    cb_body_id body;
    ca_animation_id animation;
    struct eu_ipos position;
};

void ce_tick(struct ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             struct cs_spritemap *spritemap);

#endif // CE_ENTITY_H
