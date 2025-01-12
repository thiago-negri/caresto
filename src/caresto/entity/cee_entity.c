#include <caresto/entity/cee_entity.h>
#include <caresto/system/csa_animation.h>
#include <caresto/system/csb_body.h>
#include <caresto/system/css_sprite.h>
#include <engine/el_log.h>
#include <gen/sprite_atlas.h>

static void cee_tick_beetle(struct cee_beetle *entity,
                            struct csa_animation_map *animation_map,
                            struct csb_body_map *body_map,
                            struct css_sprite_map *sprite_map) {
    struct csb_body *body = csb_get(body_map, entity->body);
    struct eo_sprite *sprite = css_get(sprite_map, entity->sprite);
    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_BEETLE];
    if (body->velocity.x != 0) {
        csa_play(&entity->animation, animation_map, GEN_ANIMATION_BEETLE_WALK,
                 entity->sprite, sprite_map);
    } else {
        csa_play(&entity->animation, animation_map, GEN_ANIMATION_BEETLE_IDLE,
                 entity->sprite, sprite_map);
    }
    entity->position.x = body->position.x - bounding_box->x;
    entity->position.y = body->position.y - bounding_box->y;
    sprite->position.x = entity->position.x;
    sprite->position.y = entity->position.y;
    if (body->velocity.x > 0.0f) {
        sprite->flags = 0;
    } else if (body->velocity.x < 0.0f) {
        sprite->flags = EO_SPRITE_MIRROR_X;
    }
}

static void cee_tick_carestosan(struct cee_carestosan *entity,
                                struct csa_animation_map *animationmap,
                                struct csb_body_map *bodymap,
                                struct css_sprite_map *spritemap) {
    struct csb_body *body = csb_get(bodymap, entity->body);
    struct eo_sprite *sprite = css_get(spritemap, entity->sprite);
    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN];

    // Carestosan animation
    if (body->velocity.y < 0.0) {
        csa_play(&entity->animation, animationmap,
                 GEN_ANIMATION_CARESTOSAN_JUMP, entity->sprite, spritemap);
    } else if (body->velocity.y > 0.0) {
        csa_play(&entity->animation, animationmap,
                 GEN_ANIMATION_CARESTOSAN_FALL, entity->sprite, spritemap);
    } else if (body->velocity.x != 0.0) {
        csa_play(&entity->animation, animationmap,
                 GEN_ANIMATION_CARESTOSAN_WALK, entity->sprite, spritemap);
    } else {
        csa_play(&entity->animation, animationmap,
                 GEN_ANIMATION_CARESTOSAN_IDLE, entity->sprite, spritemap);
    }

    entity->position.x = body->position.x - bounding_box->x;
    entity->position.y = body->position.y - bounding_box->y;
    sprite->position.x = entity->position.x;
    sprite->position.y = entity->position.y;
    if (body->velocity.x > 0.0f) {
        sprite->flags = 0;
    } else if (body->velocity.x < 0.0f) {
        sprite->flags = EO_SPRITE_MIRROR_X;
    }
}

void cee_tick(union cee_entity *entity, struct csa_animation_map *animationmap,
              struct csb_body_map *bodymap, struct css_sprite_map *spritemap) {
    switch (entity->type) {
    case cee_undefined:
        el_assert("undefined entity type");
        break;

    case cee_beetle:
        cee_tick_beetle(&entity->beetle, animationmap, bodymap, spritemap);
        break;

    case cee_carestosan:
        cee_tick_carestosan(&entity->carestosan, animationmap, bodymap,
                            spritemap);
        break;
    }
}
