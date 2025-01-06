#ifndef CA_ANIMATION_H
#define CA_ANIMATION_H

#include <stdint.h>

#include <engine/egl_opengl.h>

#include <gen/sprite_atlas.h>

#define CA_ANIMATIONS_MAX 1024

typedef size_t ca_animation_id;

struct ca_animation {
    enum gen_animation_index animation_index;
    size_t current_frame;
    uint64_t duration_remaining;
};

struct ca_animationmap {
    size_t animation_count;
    struct ca_animation animations[CA_ANIMATIONS_MAX];
};

ca_animation_id ca_play(struct ca_animationmap *animationmap,
                        enum gen_animation_index animation_index);

void ca_change(struct ca_animationmap *animationmap, ca_animation_id id,
               enum gen_animation_index animation_index);

void ca_done(struct ca_animationmap *animationmap, ca_animation_id id);

const struct gen_frame *ca_step(struct ca_animationmap *animationmap,
                                ca_animation_id id, uint64_t delta_time);

#endif // CA_ANIMATION_H
