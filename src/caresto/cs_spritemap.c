#include <caresto/ci_ids.h>
#include <caresto/cs_spritemap.h>
#include <engine/eu_utils.h>

CI_IDS(cs_spritemap, cs, ids, CS_SPRITES_MAX)

cs_sprite_id cs_add(struct cs_spritemap *spritemap, struct egl_sprite *sprite) {
    eu_assert(spritemap->sprite_count < CS_SPRITES_MAX);

    size_t index = spritemap->sprite_count;
    cs_sprite_id id = cs_ids_new(spritemap, index);

    memcpy(&spritemap->sprites[index], sprite, sizeof(struct egl_sprite));
    spritemap->sprite_count++;

    return id;
}

struct egl_sprite *cs_get(struct cs_spritemap *spritemap, cs_sprite_id id) {
    size_t index = cs_ids_get(spritemap, id);
    return &spritemap->sprites[index];
}

void cs_remove(struct cs_spritemap *spritemap, cs_sprite_id id) {
    size_t index = cs_ids_rm(spritemap, id);

    spritemap->sprite_count--;
    if (spritemap->sprite_count > 0) {
        memcpy(&spritemap->sprites[index],
               &spritemap->sprites[spritemap->sprite_count],
               sizeof(struct egl_sprite));

        cs_ids_move(spritemap, index, spritemap->sprite_count);
    }
}
