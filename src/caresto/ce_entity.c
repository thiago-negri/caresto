#include <caresto/cb_bodymap.h>
#include <caresto/ce_entity.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>

void ce_tick(struct ce_entity *entity, struct cb_bodymap *bodymap,
             struct ct_tilemap *tilemap) {
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
