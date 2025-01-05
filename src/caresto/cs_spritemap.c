#include <engine/eu_utils.h>

#include <caresto/cs_spritemap.h>

cs_sprite_id cs_add(struct cs_spritemap *spritemap, struct egl_sprite *sprite) {
    eu_assert(spritemap->sprite_count < CS_SPRITES_MAX);

    cs_sprite_id id = spritemap->sprite_count;
    memcpy(&spritemap->sprites[id], sprite, sizeof(struct egl_sprite));
    spritemap->sprite_count++;
    return id;
}

struct egl_sprite *cs_get(struct cs_spritemap *spritemap, cs_sprite_id id) {
    eu_assert(id < spritemap->sprite_count);

    return &spritemap->sprites[id];
}

void cs_remove(struct cs_spritemap *spritemap, cs_sprite_id id) {
    eu_assert(id < spritemap->sprite_count);

    spritemap->sprite_count--;
    if (spritemap->sprite_count > 0) {
        memcpy(&spritemap->sprites[id],
               &spritemap->sprites[spritemap->sprite_count],
               sizeof(struct egl_sprite));
    }
}
