#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/ce_entity.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>
#include <engine/el_log.h>
#include <gen/sprite_atlas.h>

void ce_tick(struct ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             struct cs_spritemap *spritemap) {
    struct cb_body *body = cb_get(bodymap, entity->body);
    struct egl_sprite *sprite = cs_get(spritemap, entity->sprite);
    if (body->velocity.x != 0) {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_BEETLE_WALK,
                entity->sprite, spritemap);
    } else {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_BEETLE_IDLE,
                entity->sprite, spritemap);
    }
    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_BEETLE];
    entity->position.x = body->position.x - bounding_box->x;
    entity->position.y = body->position.y - bounding_box->y;
    sprite->position.x = entity->position.x;
    sprite->position.y = entity->position.y;
    if (body->velocity.x > 0.0f) {
        sprite->flags = 0;
    } else if (body->velocity.x < 0.0f) {
        sprite->flags = EGL_SPRITE_MIRROR_X;
    }
}
