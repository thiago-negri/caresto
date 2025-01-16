#include <caresto/data/cdi_id.h>
#include <caresto/system/csa_animation.h>
#include <caresto/system/css_sprite.h>
#include <engine/et_test.h>

CDI_IDS(cds_animation_map, csa, unsigned char, ids, CDS_ANIMATIONS_MAX)

typedef unsigned char csa_animation_index;

ET_TEST(csa_sizes) {
    ET_ASSERT((csa_animation_index)CDS_ANIMATIONS_MAX == CDS_ANIMATIONS_MAX)
    ET_ASSERT((cds_animation_id)CDS_ANIMATIONS_MAX == CDS_ANIMATIONS_MAX)
    ET_ASSERT(sizeof(((struct cds_animation *)nullptr)->current_frame) >=
              sizeof(enum gen_frame_index))
    ET_DONE
}

void csa_play(struct cds_systems *systems, cds_animation_id *animation_id,
              enum gen_animation_index animation_index,
              cds_sprite_id sprite_id) {
    el_assert(systems->animation_map.animation_count < CDS_ANIMATIONS_MAX);

    bool new_animation = *animation_id == 0;

    const struct gen_animation *animation_data =
        &gen_animation_atlas[animation_index];
    const struct gen_frame *first_frame =
        &gen_frame_atlas[animation_data->from];
    size_t frame_limit = animation_data->to - animation_data->from;

    if (frame_limit < 1) {
        csa_done(systems, animation_id);
        css_set_texture(systems, sprite_id, first_frame->u, first_frame->v);
        return;
    }

    csa_animation_index index;
    if (new_animation) {
        index = systems->animation_map.animation_count;
        *animation_id = csa_ids_new(&systems->animation_map, index);
        systems->animation_map.animation_count++;
    } else {
        index = csa_ids_get(&systems->animation_map, *animation_id);
    }

    struct cds_animation *animation = &systems->animation_map.animations[index];
    if (new_animation || animation->animation_index != animation_index) {
        animation->sprite = sprite_id;
        animation->animation_index = animation_index;
        animation->current_frame = 0;
        animation->duration_remaining = first_frame->duration;
    }
}

void csa_done(struct cds_systems *systems, cds_animation_id *animation_id) {
    if (*animation_id == 0) {
        return;
    }

    size_t index = csa_ids_rm(&systems->animation_map, *animation_id);

    systems->animation_map.animation_count--;
    if (index < systems->animation_map.animation_count) {
        memcpy(&systems->animation_map.animations[index],
               &systems->animation_map
                    .animations[systems->animation_map.animation_count],
               sizeof(struct cds_animation));

        csa_ids_move(&systems->animation_map, index,
                     systems->animation_map.animation_count);
    }

    *animation_id = 0;
}

void csa_frame(struct cds_systems *systems, double delta_time) {
    for (size_t i = 0; i < systems->animation_map.animation_count; i++) {
        struct cds_animation *animation = &systems->animation_map.animations[i];
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
            css_set_texture(systems, animation->sprite, frame_data->u,
                            frame_data->v);
        }
    }
}

ET_TEST(csa_animations) {
    struct cds_systems systems = {0};

    cds_sprite_id sprites[4] = {0};

    css_set(&systems, &sprites[0], &(struct cds_sprite){0});
    css_set(&systems, &sprites[1], &(struct cds_sprite){0});
    css_set(&systems, &sprites[2], &(struct cds_sprite){0});
    css_set(&systems, &sprites[3], &(struct cds_sprite){0});

    cds_animation_id animations[4] = {0};

    csa_play(&systems, &animations[0], GEN_ANIMATION_BEETLE_WALK, sprites[0]);
    ET_ASSERT(animations[0] == 1); // new id

    csa_play(&systems, &animations[1], GEN_ANIMATION_CARESTOSAN_ATTACK_MELEE,
             sprites[1]);
    ET_ASSERT(animations[1] == 2); // new id

    csa_play(&systems, &animations[2], GEN_ANIMATION_CARESTOSAN_ATTACK_RANGED,
             sprites[2]);
    ET_ASSERT(animations[2] == 3); // new id

    csa_done(&systems, &animations[1]);
    ET_ASSERT(animations[1] == 0); // change id to zero

    csa_play(&systems, &animations[3], GEN_ANIMATION_CARESTOSAN_WALK,
             sprites[3]);
    ET_ASSERT(animations[3] == 2); // reuse id

    csa_play(&systems, &animations[1], GEN_ANIMATION_BEETLE_IDLE, sprites[1]);
    // keep as zero, animation only has one sprite
    ET_ASSERT(animations[1] == 0);

    csa_play(&systems, &animations[1], GEN_ANIMATION_BEETLE_WALK, sprites[1]);
    ET_ASSERT(animations[1] == 4); // new id

    ET_DONE;
}
