#ifndef CS_SPRITE_H
#define CS_SPRITE_H

#include <engine/egl_opengl.h>

#define CS_SPRITES_MAX 1024

typedef size_t sprite_id;

struct cs_sprite {
    size_t sprite_count;
    struct egl_sprite sprites[CS_SPRITES_MAX];
};

sprite_id cs_add(struct cs_sprite *sprites, struct egl_sprite *sprite);
struct egl_sprite *cs_get(struct cs_sprite *sprites, sprite_id id);
void cs_remove(struct cs_sprite *sprites, sprite_id id);

#endif // CS_SPRITE_H
