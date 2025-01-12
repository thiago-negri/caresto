#pragma once

#include <caresto/entity/cee_entity.h>
#include <caresto/opengl/coo_opengl.h>
#include <caresto/system/csc_camera.h>

struct cds_state {
    struct csc_camera camera;

    struct coo_sprite_shader sprite_shader;
    struct eo_sprite_buffer sprite_buffer;
    struct eo_texture sprite_atlas;

    struct coo_tile_shader tile_shader;
    struct eo_tile_buffer tile_buffer;
    struct eo_texture tile_atlas;

    double delta_time_remaining;

    struct cee_carestosan carestosan;
    struct cee_beetle beetle;

    struct csa_animation_map animationmap;
    struct csb_body_map bodymap;
    struct css_sprite_map spritemap;
    struct cst_tile_map tilemap;
};
