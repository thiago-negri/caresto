#ifndef CS_SPRITEMAP_H
#define CS_SPRITEMAP_H

#include <engine/egl_opengl.h>

#define CS_SPRITES_MAX 1024

typedef size_t sprite_id;

struct cs_spritemap {
    size_t sprite_count;
    struct egl_sprite sprites[CS_SPRITES_MAX];
};

sprite_id cs_add(struct cs_spritemap *spritemap, struct egl_sprite *sprite);
struct egl_sprite *cs_get(struct cs_spritemap *spritemap, sprite_id id);
void cs_remove(struct cs_spritemap *spritemap, sprite_id id);

#endif // CS_SPRITEMAP_H
