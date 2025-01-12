#include <caresto/data/cdi_id.h>
#include <caresto/system/csa_animation.h>
#include <caresto/system/css_sprite.h>
#include <engine/em_math.h>
#include <engine/et_test.h>
#include <gen/sprite_atlas.h>
#include <stdint.h>
#include <string.h>

CDI_IDS(csa_animation_map, csa, ids, CSA_ANIMATIONS_MAX)

void csa_play(csa_animation_id *animation_id,
              struct csa_animation_map *animationmap,
              enum gen_animation_index animation_index, css_sprite_id sprite_id,
              struct css_sprite_map *spritemap) {
    el_assert(animationmap->animation_count < CSA_ANIMATIONS_MAX);

    bool new_animation = *animation_id == 0;

    const struct gen_animation *animation_data =
        &gen_animation_atlas[animation_index];
    const struct gen_frame *first_frame =
        &gen_frame_atlas[animation_data->from];
    size_t frame_limit = animation_data->to - animation_data->from;

    if (frame_limit < 1) {
        csa_done(animation_id, animationmap);

        struct eo_sprite *sprite = css_get(spritemap, sprite_id);
        sprite->texture_offset.u = first_frame->u;
        sprite->texture_offset.v = first_frame->v;
        return;
    }

    size_t index;
    if (new_animation) {
        index = animationmap->animation_count;
        *animation_id = csa_ids_new(animationmap, index);
        animationmap->animation_count++;
    } else {
        index = csa_ids_get(animationmap, *animation_id);
    }

    struct csa_animation *animation = &animationmap->animations[index];
    if (new_animation || animation->animation_index != animation_index) {
        animation->sprite = sprite_id;
        animation->animation_index = animation_index;
        animation->current_frame = 0;
        animation->duration_remaining = first_frame->duration;
    }
}

void csa_done(csa_animation_id *animation_id,
              struct csa_animation_map *animationmap) {
    if (*animation_id == 0) {
        return;
    }

    size_t index = csa_ids_rm(animationmap, *animation_id);

    animationmap->animation_count--;
    if (animationmap->animation_count > 0) {
        memcpy(&animationmap->animations[index],
               &animationmap->animations[animationmap->animation_count],
               sizeof(struct csa_animation));

        csa_ids_move(animationmap, index, animationmap->animation_count);
    }

    *animation_id = 0;
}

void csa_frame(struct csa_animation_map *animationmap, double delta_time,
               struct css_sprite_map *spritemap) {
    for (size_t i = 0; i < animationmap->animation_count; i++) {
        struct csa_animation *animation = &animationmap->animations[i];
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
            struct eo_sprite *sprite = css_get(spritemap, animation->sprite);
            sprite->texture_offset.u = frame_data->u;
            sprite->texture_offset.v = frame_data->v;
        }
    }
}

ET_TEST(animations) {
    struct csa_animation_map animationmap = {0};
    struct css_sprite_map spritemap = {0};

    css_sprite_id sprites[4] = {
        css_add(&spritemap, &(struct eo_sprite){0}),
        css_add(&spritemap, &(struct eo_sprite){0}),
        css_add(&spritemap, &(struct eo_sprite){0}),
        css_add(&spritemap, &(struct eo_sprite){0}),
    };

    csa_animation_id animations[4] = {0};

    csa_play(&animations[0], &animationmap, GEN_ANIMATION_BEETLE_WALK,
             sprites[0], &spritemap);
    ET_ASSERT(animations[0] == 1); // new id

    csa_play(&animations[1], &animationmap,
             GEN_ANIMATION_CARESTOSAN_ATTACK_MELEE, sprites[1], &spritemap);
    ET_ASSERT(animations[1] == 2); // new id

    csa_play(&animations[2], &animationmap,
             GEN_ANIMATION_CARESTOSAN_ATTACK_RANGED, sprites[2], &spritemap);
    ET_ASSERT(animations[2] == 3); // new id

    csa_done(&animations[1], &animationmap);
    ET_ASSERT(animations[1] == 0); // change id to zero

    csa_play(&animations[3], &animationmap, GEN_ANIMATION_CARESTOSAN_WALK,
             sprites[3], &spritemap);
    ET_ASSERT(animations[3] == 2); // reuse id

    csa_play(&animations[1], &animationmap, GEN_ANIMATION_BEETLE_IDLE,
             sprites[1], &spritemap);
    // keep as zero, animation only has one sprite
    ET_ASSERT(animations[1] == 0);

    csa_play(&animations[1], &animationmap, GEN_ANIMATION_BEETLE_WALK,
             sprites[1], &spritemap);
    ET_ASSERT(animations[1] == 4); // new id

    ET_DONE;
}

ET_TEST(csa_sizes) {
    ET_ASSERT(sizeof(((struct csa_animation *)nullptr)->current_frame) >=
              sizeof(enum gen_frame_index))
    ET_DONE
}
