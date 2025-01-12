#pragma once

#include <engine/ea_allocator.h>
#include <engine/em_math.h>
#include <engine/eo_opengl.h>

struct coo_sprite_shader {
    GLuint program_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

struct coo_tile_shader {
    GLuint program_id;
    GLint g_tile_size_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

int coo_sprite_shader_load(struct coo_sprite_shader *out_shader,
                           struct ea_arena *arena);

void coo_sprite_shader_destroy(struct coo_sprite_shader *shader);

void coo_sprite_shader_render(struct coo_sprite_shader *shader,
                              struct em_mat4 *g_transform_mat,
                              struct eo_texture *texture, int sprite_count,
                              struct eo_sprite_buffer *sprite_buffer);

int coo_tile_shader_load(struct coo_tile_shader *out_shader,
                         struct ea_arena *arena);

void coo_tile_shader_destroy(struct coo_tile_shader *shader);

void coo_tile_shader_render(struct coo_tile_shader *shader,
                            struct em_isize *g_tile_size,
                            struct em_mat4 *g_transform_mat,
                            struct eo_texture *texture, int tile_count,
                            struct eo_tile_buffer *tile_buffer);
