#include <engine/eu_utils.h>

#include <caresto/cs_sprite.h>

sprite_id cs_add(struct cs_sprite *sprites, struct egl_sprite *sprite) {
    eu_assert(sprites->sprite_count < CT_SPRITE_MAX);

    sprite_id id = sprites->sprite_count;
    memcpy(&sprites->sprites[id], sprite, sizeof(struct egl_sprite));
    sprites->sprite_count++;
    return id;
}

struct egl_sprite *cs_get(struct cs_sprite *sprites, sprite_id id) {
    eu_assert(id < sprites->sprite_count);

    return &sprites->sprites[id];
}

void cs_remove(struct cs_sprite *sprites, sprite_id id) {
    eu_assert(id < sprites->sprite_count);

    sprites->sprite_count--;
    if (sprites->sprite_count > 0) {
        memcpy(&sprites->sprites[id], &sprites->sprites[sprites->sprite_count],
               sizeof(struct egl_sprite));
    }
}
