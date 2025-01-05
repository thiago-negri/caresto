#include <stdint.h>
#include <string.h>

#include <engine/eu_utils.h>

#include <caresto/ca_animation.h>

static struct ca_frame *ca_add_frames(struct ca_animationmap *animationmap,
                                      size_t frame_count,
                                      struct ca_frame *frames) {
    eu_assert(animationmap->frame_count + frame_count <= CA_FRAMES_MAX);

    struct ca_frame *result = &animationmap->frames[animationmap->frame_count];

    memcpy(&animationmap[animationmap->frame_count], frames,
           frame_count * sizeof(struct ca_frame));

    animationmap->frame_count = animationmap->frame_count + frame_count;

    return result;
}

ca_animation_id ca_add(struct ca_animationmap *animationmap,
                       uint64_t frame_duration, struct eu_ivec2 *frame_size,
                       size_t frame_count, struct ca_frame *frames) {
    eu_assert(animationmap->animation_count < CA_ANIMATIONS_MAX);

    struct ca_frame *first_fame =
        ca_add_frames(animationmap, frame_count, frames);

    ca_animation_id id = animationmap->animation_count;

    animationmap->animations[id] = (struct ca_animation){
        .frame_size = *frame_size,
        .frame_duration = frame_duration,
        .frame_count = frame_count,
        .frames = first_fame,
    };

    animationmap->animation_count++;
    return id;
}

ca_animation_instance_id ca_play(struct ca_animationmap *animationmap,
                                 ca_animation_id id) {
    eu_assert(animationmap->animation_instance_count <
              CA_ANIMATION_INSTANCES_MAX);
    eu_assert(id < animationmap->animation_count);

    ca_animation_instance_id instance_id =
        animationmap->animation_instance_count;

    struct ca_animation *animation = &animationmap->animations[id];

    animationmap->animation_instances[instance_id] =
        (struct ca_animation_instance){
            .current_frame = 0,
            .duration_remaining = animation->frame_duration,
            .animation = animation,
        };

    animationmap->animation_instance_count++;
    return instance_id;
}

void ca_change(struct ca_animationmap *animationmap,
               ca_animation_instance_id instance_id, ca_animation_id id) {
    eu_assert(instance_id < animationmap->animation_instance_count);
    eu_assert(id < animationmap->animation_count);

    struct ca_animation_instance *instance =
        &animationmap->animation_instances[instance_id];
    struct ca_animation *animation = &animationmap->animations[id];

    // FIXME(tnegri): #35 What if we want to reset the current animation?
    if (instance->animation != animation) {
        instance->current_frame = 0;
        instance->duration_remaining = animation->frame_duration;
        instance->animation = animation;
    }
}

void ca_done(struct ca_animationmap *animationmap,
             ca_animation_instance_id instance_id) {
    eu_assert(instance_id < animationmap->animation_instance_count);

    animationmap->animation_instance_count--;
    if (animationmap->animation_instance_count > 0) {
        memcpy(
            &animationmap->animation_instances[instance_id],
            &animationmap
                 ->animation_instances[animationmap->animation_instance_count],
            sizeof(struct ca_animation_instance));
    }
}

void ca_remove(struct ca_animationmap *animationmap, ca_animation_id id) {
    eu_assert(id < animationmap->animation_count);

    animationmap->animation_count--;
    if (animationmap->animation_count > 0) {
        memcpy(&animationmap->animations[id],
               &animationmap->animations[animationmap->animation_count],
               sizeof(struct ca_animation));
    }
}

struct ca_frame *ca_frame(struct ca_animationmap *animationmap,
                          ca_animation_instance_id instance_id,
                          uint64_t delta_time) {
    eu_assert(instance_id < animationmap->animation_instance_count);

    struct ca_animation_instance *instance =
        &animationmap->animation_instances[instance_id];

    while (instance->duration_remaining < delta_time) {
        delta_time -= instance->duration_remaining;
        if (instance->current_frame < instance->animation->frame_count - 1) {
            instance->current_frame += 1;
        } else {
            instance->current_frame = 0;
        }
        instance->duration_remaining = instance->animation->frame_duration;
    }

    instance->duration_remaining -= delta_time;

    return &instance->animation->frames[instance->current_frame];
}
