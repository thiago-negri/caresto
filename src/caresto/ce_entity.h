#ifndef CE_ENTITY_H
#define CE_ENTITY_H

#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/cs_spritemap.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>

enum ce_type {
    ce_undefined,
    ce_beetle,
    ce_carestosan,
};

struct ce_beetle {
    enum ce_type type;
    cs_sprite_id sprite;
    cb_body_id body;
    ca_animation_id animation;
    struct eu_ivec2 position;
};

struct ce_carestosan {
    enum ce_type type;
    cs_sprite_id sprite;
    cb_body_id body;
    ca_animation_id animation;
    struct eu_ivec2 position;
};

union ce_entity {
    enum ce_type type;
    struct ce_beetle beetle;
    struct ce_carestosan carestosan;
};

void ce_tick(union ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct cs_spritemap *spritemap);

#endif // CE_ENTITY_H
