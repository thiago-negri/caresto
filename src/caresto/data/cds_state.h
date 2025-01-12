#pragma once

#include <caresto/entity/cee_entity.h>
#include <caresto/opengl/coo_opengl.h>
#include <caresto/system/csc_camera.h>

struct cds_state {
    struct csc_camera camera;

#ifdef DEBUG
    /*struct coo_debug_shader debug_shader;*/
    /*struct eo_buffer debug_buffer;*/
#endif // DEBUG

    struct coo_sprite_shader sprite_shader;
    struct eo_buffer sprite_buffer;
    struct eo_buffer tile_buffer;
    struct eo_texture sprite_atlas;
    struct eo_texture tile_atlas;

    double delta_time_remaining;

    struct cee_carestosan carestosan;
    struct cee_beetle beetle;

    struct csa_animation_map animationmap;
    struct csb_body_map bodymap;
    struct css_sprite_map spritemap;
    struct cst_tile_map tilemap;
};
