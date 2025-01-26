#pragma once

#include <caresto/data/cds_systems.h>

struct cdd_data {
#ifdef DEBUG
    struct coo_debug_shader debug_shader;
    struct eo_buffer debug_buffer;
#endif // DEBUG

    unsigned int hello_world_texture_id;

    struct coo_sprite_shader sprite_shader;
    struct eo_buffer sprite_buffer;
    struct eo_buffer tile_buffer;
    struct eo_texture sprite_atlas;
    struct eo_texture tile_atlas;

    double delta_time_remaining;

    struct cds_systems systems;
};
