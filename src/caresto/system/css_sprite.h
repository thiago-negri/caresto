#pragma once

#include <caresto/opengl/coo_opengl.h>
#include <gen/sprite_atlas.h>

#define CSS_SPRITES_MAX 100

typedef unsigned char css_sprite_id;

#define CSS_MIRROR_X 1

struct css_sprite {
    int x, y;
    int w, h;
    int u, v;
    unsigned int flags;
};

struct css_sprite_map {
    unsigned char sprite_count;
    unsigned char ids[CSS_SPRITES_MAX];
    struct css_sprite sprites[CSS_SPRITES_MAX];
    struct coo_sprite sprites_gpu[CSS_SPRITES_MAX];
};

void css_set(struct css_sprite_map *spritemap, css_sprite_id *id,
             struct css_sprite *sprite);

void css_set_texture(struct css_sprite_map *spritemap, css_sprite_id id, int u,
                     int v);

void css_set_position(struct css_sprite_map *spritemap, css_sprite_id id, int x,
                      int y);

void css_set_flags(struct css_sprite_map *spritemap, css_sprite_id id, unsigned int flags);

void css_remove(struct css_sprite_map *spritemap, css_sprite_id *id);
