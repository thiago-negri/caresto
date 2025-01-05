#include <caresto/cb_bodymap.h>
#include <caresto/ce_entity.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>

#define GRAVITY_MAX_VELOCITY 5.0f
#define GRAVITY_ACCELERATION_PER_TICK 0.05f

void ce_tick(struct ce_entity *entity, struct cb_bodymap *bodymap,
             struct ct_tilemap *tilemap) {
    // FIXME(tnegri): Combine move and grounded into a single pass
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
        // FIXME(tnegri): Make move still apply X even if Y is blocked for
        // example, and also apply per pixel, even if the attempted movement
        // is bigger and 1px
        bool moved = cb_move(bodymap, tilemap, entity->body, &movement);
        if (moved) {
            entity->position.x += movement.x;
            entity->position.y += movement.y;
        } else {
            // FIXME(tnegri): Until we fix the previous comment this makes sure
            // we can still move
            entity->velocity.y = 0.0f;
        }
    }

    entity->movement_remaining.x -= movement.x;
    entity->movement_remaining.y -= movement.y;
}

void ce_frame(struct ce_entity *entity, struct cs_spritemap *spritemap) {
    struct egl_sprite *sprite = cs_get(spritemap, entity->sprite);
    sprite->position.x = (int)entity->position.x;
    sprite->position.y = (int)entity->position.y;
}
