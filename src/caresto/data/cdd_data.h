#pragma once

#include <caresto/data/cds_systems.h>

struct cdd_data {
#ifdef DEBUG
    struct coo_debug_shader debug_shader;
    struct eo_buffer debug_buffer;
#endif // DEBUG

    // TODO(tnegri): Split state that can be reloaded from disk
    // and state that needs to be recreated (font, shaders)

    struct eo_buffer text_buffer;

    struct coo_sprite_shader sprite_shader;
    struct eo_buffer sprite_buffer;
    struct eo_buffer tile_buffer;
    struct eo_texture sprite_atlas;
    struct eo_texture tile_atlas;

    double delta_time_remaining;

    struct cds_systems systems;
};
