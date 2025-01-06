#include <caresto/ca_animation.h>
#include <engine/eu_utils.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>
#include <string.h>

ca_animation_id ca_play(struct ca_animationmap *animationmap,
                        enum gen_animation_index animation_index) {
    eu_assert(animationmap->animation_count < CA_ANIMATIONS_MAX);

    ca_animation_id id = animationmap->animation_count;

    struct ca_animation *animation = &animationmap->animations[id];
    animation->animation_index = animation_index;
    animation->current_frame = 0;
    enum gen_frame_index first_frame =
        gen_animation_atlas[animation_index].from;
    animation->duration_remaining = gen_frame_atlas[first_frame].duration;

    animationmap->animation_count++;
    return id;
}

void ca_change(struct ca_animationmap *animationmap, ca_animation_id id,
               enum gen_animation_index animation_index) {
    eu_assert(id < animationmap->animation_count);

    struct ca_animation *animation = &animationmap->animations[id];

    // FIXME(tnegri): #35 What if we want to reset the current animation?
    if (animation->animation_index != animation_index) {
        animation->animation_index = animation_index;
        animation->current_frame = 0;

        enum gen_frame_index first_frame =
            gen_animation_atlas[animation_index].from;
        animation->duration_remaining = gen_frame_atlas[first_frame].duration;
    }
}

void ca_done(struct ca_animationmap *animationmap, ca_animation_id id) {
    eu_assert(id < animationmap->animation_count);

    animationmap->animation_count--;
    if (animationmap->animation_count > 0) {
        memcpy(&animationmap->animations[id],
               &animationmap->animations[animationmap->animation_count],
               sizeof(struct ca_animation));
    }
}

const struct gen_frame *ca_step(struct ca_animationmap *animationmap,
                                ca_animation_id id, uint64_t delta_time) {
    eu_assert(id < animationmap->animation_count);

    struct ca_animation *animation = &animationmap->animations[id];
    struct gen_animation animation_data =
        gen_animation_atlas[animation->animation_index];

    while (animation->duration_remaining < delta_time) {
        delta_time -= animation->duration_remaining;
        size_t frame_limit = animation_data.to - animation_data.from;
        if (animation->current_frame < frame_limit) {
            animation->current_frame += 1;
        } else {
            animation->current_frame = 0;
        }

        animation->duration_remaining =
            gen_frame_atlas[animation_data.from + animation->current_frame]
                .duration;
    }

    animation->duration_remaining -= delta_time;

    return &gen_frame_atlas[animation_data.from + animation->current_frame];
}
