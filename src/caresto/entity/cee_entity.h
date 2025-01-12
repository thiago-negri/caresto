#pragma once

#include <caresto/system/csa_animation.h>
#include <caresto/system/csb_body.h>
#include <caresto/system/css_sprite.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>

enum cee_type {
    cee_undefined,
    cee_beetle,
    cee_carestosan,
};

struct cee_beetle {
    enum cee_type type;
    css_sprite_id sprite;
    csb_body_id body;
    csa_animation_id animation;
    struct em_ivec2 position;
};

struct cee_carestosan {
    enum cee_type type;
    css_sprite_id sprite;
    csb_body_id body;
    csa_animation_id animation;
    struct em_ivec2 position;
};

union cee_entity {
    enum cee_type type;
    struct cee_beetle beetle;
    struct cee_carestosan carestosan;
};

void cee_tick(union cee_entity *entity, struct csa_animation_map *animationmap,
              struct csb_body_map *bodymap, struct css_sprite_map *spritemap);
