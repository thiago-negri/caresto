#ifndef CD_DATA_H
#define CD_DATA_H

#include <caresto/cc_camera.h>
#include <caresto/ce_entity.h>
#include <caresto/cgl_opengl.h>

struct cd_state {
    // Game camera
    struct cc_camera camera;

    // Sprite shader
    struct cgl_sprite_shader sprite_shader;
    struct egl_sprite_buffer sprite_buffer;
    struct egl_texture sprite_atlas;

    // Tile shader
    struct cgl_tile_shader tile_shader;
    struct egl_tile_buffer tile_buffer;
    struct egl_texture tile_atlas;

    double delta_time_remaining;

    struct ce_carestosan carestosan;
    struct ce_beetle beetle;

    struct cs_spritemap spritemap;
    struct ct_tilemap tilemap;
    struct cb_bodymap bodymap;
    struct ca_animationmap animationmap;
};

#endif // CD_DATA_H
