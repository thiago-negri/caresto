#pragma once

#include <engine/eo_opengl.h>
#include <gen/sprite_atlas.h>

#define CSS_SPRITES_MAX 100

typedef unsigned char css_sprite_id;

struct css_sprite_map {
    unsigned char sprite_count;
    unsigned char ids[CSS_SPRITES_MAX];
    struct eo_sprite sprites[CSS_SPRITES_MAX];
};

css_sprite_id css_add(struct css_sprite_map *spritemap,
                      struct eo_sprite *sprite);
struct eo_sprite *css_get(struct css_sprite_map *spritemap, css_sprite_id id);
void css_remove(struct css_sprite_map *spritemap, css_sprite_id id);
