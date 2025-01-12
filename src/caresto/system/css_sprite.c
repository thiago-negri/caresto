#include <caresto/data/cdi_id.h>
#include <caresto/system/css_sprite.h>
#include <engine/el_log.h>

CDI_IDS(css_sprite_map, css, ids, CSS_SPRITES_MAX)

css_sprite_id css_add(struct css_sprite_map *spritemap,
                      struct eo_sprite *sprite) {
    el_assert(spritemap->sprite_count < CSS_SPRITES_MAX);

    size_t index = spritemap->sprite_count;
    css_sprite_id id = css_ids_new(spritemap, index);

    memcpy(&spritemap->sprites[index], sprite, sizeof(struct eo_sprite));
    spritemap->sprite_count++;

    return id;
}

struct eo_sprite *css_get(struct css_sprite_map *spritemap, css_sprite_id id) {
    size_t index = css_ids_get(spritemap, id);
    return &spritemap->sprites[index];
}

void css_remove(struct css_sprite_map *spritemap, css_sprite_id id) {
    size_t index = css_ids_rm(spritemap, id);

    spritemap->sprite_count--;
    if (spritemap->sprite_count > 0) {
        memcpy(&spritemap->sprites[index],
               &spritemap->sprites[spritemap->sprite_count],
               sizeof(struct eo_sprite));

        css_ids_move(spritemap, index, spritemap->sprite_count);
    }
}
