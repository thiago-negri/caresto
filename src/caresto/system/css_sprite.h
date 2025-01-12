#pragma once

#include <engine/eo_opengl.h>
#include <gen/sprite_atlas.h>

#define CSS_SPRITES_MAX 1024

typedef size_t css_sprite_id;

struct css_sprite_map {
    int sprite_count;
    size_t ids[CSS_SPRITES_MAX];
    struct eo_sprite sprites[CSS_SPRITES_MAX];
};

css_sprite_id css_add(struct css_sprite_map *spritemap,
                      struct eo_sprite *sprite);
struct eo_sprite *css_get(struct css_sprite_map *spritemap, css_sprite_id id);
void css_remove(struct css_sprite_map *spritemap, css_sprite_id id);
