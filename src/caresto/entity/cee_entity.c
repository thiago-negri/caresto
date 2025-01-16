#include <caresto/entity/cee_entity.h>
#include <caresto/system/csa_animation.h>
#include <caresto/system/csb_body.h>
#include <caresto/system/css_sprite.h>
#include <engine/el_log.h>
#include <gen/sprite_atlas.h>

static void cee_tick_beetle(struct cds_beetle *entity,
                            struct cds_systems *systems) {
    struct cds_body *body = csb_get(systems, entity->body);

    if (body->velocity.x != 0) {
        csa_play(systems, &entity->animation, GEN_ANIMATION_BEETLE_WALK,
                 entity->sprite);
    } else {
        csa_play(systems, &entity->animation, GEN_ANIMATION_BEETLE_IDLE,
                 entity->sprite);
    }

    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_BEETLE];
    entity->position.x = body->position.x - bounding_box->x;
    entity->position.y = body->position.y - bounding_box->y;

    // FIXME(tnegri): Updating coo_sprite twice

    if (body->velocity.x > 0.0f) {
        css_set_flags(systems, entity->sprite, 0);
    } else if (body->velocity.x < 0.0f) {
        css_set_flags(systems, entity->sprite, CDS_SPRITE_MIRROR_X);
    }

    css_set_position(systems, entity->sprite, entity->position.x,
                     entity->position.y);
}

static void cee_tick_carestosan(struct cds_carestosan *entity,
                                struct cds_systems *systems) {
    struct cds_body *body = csb_get(systems, entity->body);

    // Carestosan animation
    if (body->velocity.y < 0.0) {
        csa_play(systems, &entity->animation, GEN_ANIMATION_CARESTOSAN_JUMP,
                 entity->sprite);
    } else if (body->velocity.y > 0.0) {
        csa_play(systems, &entity->animation, GEN_ANIMATION_CARESTOSAN_FALL,
                 entity->sprite);
    } else if (body->velocity.x != 0.0) {
        csa_play(systems, &entity->animation, GEN_ANIMATION_CARESTOSAN_WALK,
                 entity->sprite);
    } else {
        csa_play(systems, &entity->animation, GEN_ANIMATION_CARESTOSAN_IDLE,
                 entity->sprite);
    }

    const struct gen_bounding_box *bounding_box =
        &gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN];
    entity->position.x = body->position.x - bounding_box->x;
    entity->position.y = body->position.y - bounding_box->y;

    // FIXME(tnegri): Updating coo_sprite twice

    if (body->velocity.x > 0.0f) {
        css_set_flags(systems, entity->sprite, 0);
    } else if (body->velocity.x < 0.0f) {
        css_set_flags(systems, entity->sprite, CDS_SPRITE_MIRROR_X);
    }

    css_set_position(systems, entity->sprite, entity->position.x,
                     entity->position.y);
}

void cee_tick(union cds_entity *entity, struct cds_systems *systems) {
    switch (entity->type) {
    case cds_entity_undefined:
        el_assert("undefined entity type");
        break;

    case cds_entity_beetle:
        cee_tick_beetle(&entity->beetle, systems);
        break;

    case cds_entity_carestosan:
        cee_tick_carestosan(&entity->carestosan, systems);
        break;
    }
}
