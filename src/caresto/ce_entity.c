#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/ce_entity.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>
#include <engine/el_log.h>
#include <gen/sprite_atlas.h>

static void ce_tick_beetle(struct ce_beetle *entity,
                           struct ca_animationmap *animationmap,
                           struct cb_bodymap *bodymap,
                           struct ct_tilemap *tilemap,
                           struct cs_spritemap *spritemap) {
    struct cb_body *body = cb_get(bodymap, entity->body);
    struct egl_sprite *sprite = cs_get(spritemap, entity->sprite);
    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_BEETLE];
    if (body->velocity.x != 0) {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_BEETLE_WALK,
                entity->sprite, spritemap);
    } else {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_BEETLE_IDLE,
                entity->sprite, spritemap);
    }
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

static void ce_tick_carestosan(struct ce_carestosan *entity,
                               struct ca_animationmap *animationmap,
                               struct cb_bodymap *bodymap,
                               struct ct_tilemap *tilemap,
                               struct cs_spritemap *spritemap) {
    struct cb_body *body = cb_get(bodymap, entity->body);
    struct egl_sprite *sprite = cs_get(spritemap, entity->sprite);
    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN];

    // Carestosan animation
    if (body->velocity.y < 0.0) {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_CARESTOSAN_JUMP,
                entity->sprite, spritemap);
    } else if (body->velocity.y > 0.0) {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_CARESTOSAN_FALL,
                entity->sprite, spritemap);
    } else if (body->velocity.x != 0.0) {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_CARESTOSAN_WALK,
                entity->sprite, spritemap);
    } else {
        ca_play(&entity->animation, animationmap, GEN_ANIMATION_CARESTOSAN_IDLE,
                entity->sprite, spritemap);
    }

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

void ce_tick(union ce_entity *entity, struct ca_animationmap *animationmap,
             struct cb_bodymap *bodymap, struct ct_tilemap *tilemap,
             struct cs_spritemap *spritemap) {
    switch (entity->type) {
    case ce_undefined:
        eu_assert("undefined entity type");
        break;

    case ce_beetle:
        ce_tick_beetle(&entity->beetle, animationmap, bodymap, tilemap,
                       spritemap);
        break;

    case ce_carestosan:
        ce_tick_carestosan(&entity->carestosan, animationmap, bodymap, tilemap,
                           spritemap);
        break;
    }
}
