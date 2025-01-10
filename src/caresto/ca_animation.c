#include <caresto/ca_animation.h>
#include <engine/eu_utils.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>
#include <string.h>

void ca_play(ca_animation_id *animation_id,
             struct ca_animationmap *animationmap,
             enum gen_animation_index animation_index, cs_sprite_id sprite_id,
             struct cs_spritemap *spritemap) {
    eu_assert(animationmap->animation_count < CA_ANIMATIONS_MAX);

    bool new_animation = *animation_id == CA_ANIMATION_NONE;

    const struct gen_animation *animation_data =
        &gen_animation_atlas[animation_index];
    const struct gen_frame *first_frame =
        &gen_frame_atlas[animation_data->from];
    size_t frame_limit = animation_data->to - animation_data->from;

    if (frame_limit < 1) {
        ca_done(animation_id, animationmap);

        struct egl_sprite *sprite = cs_get(spritemap, sprite_id);
        sprite->texture_offset.u = first_frame->u;
        sprite->texture_offset.v = first_frame->v;
        return;
    }

    if (new_animation) {
        *animation_id = animationmap->animation_count + 1;
        animationmap->animation_count++;
    }

    ca_animation_id id = *animation_id;

    eu_assert_fmt(id > 0 && id <= animationmap->animation_count,
                  "%llu / %llu\n", id, animationmap->animation_count);

    struct ca_animation *animation = &animationmap->animations[id - 1];
    if (new_animation || animation->animation_index != animation_index) {
        animation->sprite = sprite_id;
        animation->animation_index = animation_index;
        animation->current_frame = 0;
        animation->duration_remaining = first_frame->duration;
    }
}

void ca_done(ca_animation_id *animation_id,
             struct ca_animationmap *animationmap) {
    if (*animation_id == CA_ANIMATION_NONE ||
        *animation_id > animationmap->animation_count) {
        return;
    }

    animationmap->animation_count--;
    if (animationmap->animation_count > 0) {
        memcpy(&animationmap->animations[*animation_id - 1],
               &animationmap->animations[animationmap->animation_count],
               sizeof(struct ca_animation));
    }

    *animation_id = CA_ANIMATION_NONE;
}

void ca_frame(struct ca_animationmap *animationmap, double delta_time,
              struct cs_spritemap *spritemap) {
    for (size_t i = 0; i < animationmap->animation_count; i++) {
        struct ca_animation *animation = &animationmap->animations[i];
        const struct gen_animation *animation_data =
            &gen_animation_atlas[animation->animation_index];
        size_t frame_limit = animation_data->to - animation_data->from;
        const struct gen_frame *frame_data = NULL;

        while (animation->duration_remaining < delta_time) {
            delta_time -= animation->duration_remaining;
            if (animation->current_frame < frame_limit) {
                animation->current_frame += 1;
            } else {
                animation->current_frame = 0;
            }

            frame_data = &gen_frame_atlas[animation_data->from +
                                          animation->current_frame];

            animation->duration_remaining = frame_data->duration;
        }

        animation->duration_remaining -= delta_time;

        if (frame_data != NULL) {
            struct egl_sprite *sprite = cs_get(spritemap, animation->sprite);
            sprite->texture_offset.u = frame_data->u;
            sprite->texture_offset.v = frame_data->v;
        }
    }
}
