#pragma once

#include <caresto/system/css_sprite.h>

#define CSA_ANIMATIONS_MAX 100

typedef unsigned char csa_animation_id;

struct csa_animation {
    css_sprite_id sprite;
    enum gen_animation_index animation_index;
    unsigned char current_frame;
    double duration_remaining;
};

struct csa_animation_map {
    unsigned char animation_count;
    unsigned char ids[CSA_ANIMATIONS_MAX];
    struct csa_animation animations[CSA_ANIMATIONS_MAX];
};

void csa_play(csa_animation_id *id, struct csa_animation_map *animation_map,
              enum gen_animation_index animation_index, css_sprite_id sprite_id,
              struct css_sprite_map *spritemap);

void csa_done(csa_animation_id *id, struct csa_animation_map *animation_map);

void csa_frame(struct csa_animation_map *animation_map, double delta_time,
               struct css_sprite_map *spritemap);
