#ifndef CA_ANIMATION_H
#define CA_ANIMATION_H

#include <engine/egl_opengl.h>
#include <stdint.h>

#define CA_FRAMES_MAX 1024
#define CA_ANIMATIONS_MAX 1024
#define CA_ANIMATION_INSTANCES_MAX 1024

typedef size_t ca_frame_id;
typedef size_t ca_animation_id;
typedef size_t ca_animation_instance_id;

struct ca_frame {
    struct eu_ivec2 texture_offset;
};

struct ca_animation {
    uint64_t frame_duration;
    struct eu_ivec2 frame_size;
    size_t frame_count;
    struct ca_frame *frames;
};

struct ca_animation_instance {
    size_t current_frame;
    uint64_t duration_remaining;
    struct ca_animation *animation;
};

struct ca_animationmap {
    size_t frame_count;
    struct ca_frame frames[CA_FRAMES_MAX];

    size_t animation_count;
    struct ca_animation animations[CA_ANIMATIONS_MAX];

    size_t animation_instance_count;
    struct ca_animation_instance animation_instances[CA_ANIMATIONS_MAX];
};

ca_animation_id ca_add(struct ca_animationmap *animationmap,
                       uint64_t frame_duration, struct eu_ivec2 *frame_size,
                       size_t frame_count, struct ca_frame *frames);

ca_animation_instance_id ca_play(struct ca_animationmap *animationmap,
                                 ca_animation_id id);

void ca_change(struct ca_animationmap *animationmap,
               ca_animation_instance_id instance_id, ca_animation_id id);

void ca_done(struct ca_animationmap *animationmap,
             ca_animation_instance_id instance_id);

void ca_remove(struct ca_animationmap *animationmap, ca_animation_id id);

struct ca_frame *ca_frame(struct ca_animationmap *animationmap,
                          ca_animation_instance_id instance_id,
                          uint64_t delta_time);

#endif // CA_ANIMATION_H
