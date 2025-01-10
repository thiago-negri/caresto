#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/ce_entity.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>
#include <engine/el_log.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

void ce_tick(struct ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             struct cs_spritemap *spritemap) {
    if (entity->velocity.y >= 0 &&
        cb_grounded(bodymap, tilemap, entity->body)) {
        entity->velocity.y = 0;
    } else {
        entity->velocity.y += GRAVITY_ACCELERATION_PER_TICK;
        if (entity->velocity.y > GRAVITY_MAX_VELOCITY) {
            entity->velocity.y = GRAVITY_MAX_VELOCITY;
        }
    }

    entity->movement_remaining.x += entity->velocity.x;
    entity->movement_remaining.y += entity->velocity.y;

    struct eu_ivec2 movement = {
        .x = (int)entity->movement_remaining.x,
        .y = (int)entity->movement_remaining.y,
    };

    if (movement.x != 0 || movement.y != 0) {
        bool moved = cb_move(bodymap, tilemap, entity->body, &movement);
        if (moved) {
            entity->position.x += movement.x;
            entity->position.y += movement.y;
        } else {
            entity->velocity.y = 0.0f;
        }
    }

    entity->movement_remaining.x -= movement.x;
    entity->movement_remaining.y -= movement.y;

    if (entity->velocity.x != 0) {
        ca_play(&entity->animation, animationmap, entity->animation_walk,
                entity->sprite, spritemap);
    } else {
        ca_play(&entity->animation, animationmap, entity->animation_idle,
                entity->sprite, spritemap);
    }
}

void ce_frame(struct ce_entity *entity, struct cs_spritemap *spritemap) {
    struct egl_sprite *sprite = cs_get(spritemap, entity->sprite);
    sprite->position.x = entity->position.x;
    sprite->position.y = entity->position.y;
    if (entity->velocity.x > 0.0f) {
        sprite->flags = 0;
    } else if (entity->velocity.x < 0.0f) {
        sprite->flags = EGL_SPRITE_MIRROR_X;
    }
}
