#ifndef CA_ANIMATION_H
#define CA_ANIMATION_H

#include <caresto/cs_spritemap.h>
#include <engine/egl_opengl.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>

#define CA_ANIMATIONS_MAX 1024
#define CA_ANIMATION_NONE 0

typedef size_t ca_animation_id;

struct ca_animation {
    cs_sprite_id sprite;
    enum gen_animation_index animation_index;
    size_t current_frame;
    double duration_remaining;
};

struct ca_animationmap {
    size_t animation_count;
    struct ca_animation animations[CA_ANIMATIONS_MAX];
};

void ca_play(ca_animation_id *id, struct ca_animationmap *animationmap,
             enum gen_animation_index animation_index, cs_sprite_id sprite_id,
             struct cs_spritemap *spritemap);

void ca_done(ca_animation_id *id, struct ca_animationmap *animationmap);

void ca_frame(struct ca_animationmap *animationmap, double delta_time,
              struct cs_spritemap *spritemap);

#endif // CA_ANIMATION_H
