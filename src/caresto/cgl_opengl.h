#ifndef CGL_OPENGL_H
#define CGL_OPENGL_H

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

struct cgl_sprite_shader {
    GLuint program_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

struct cgl_tile_shader {
    GLuint program_id;
    GLint g_tile_size_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

int cgl_sprite_shader_load(struct cgl_sprite_shader *out_shader,
                           struct em_arena *arena);

void cgl_sprite_shader_destroy(struct cgl_sprite_shader *shader);

void cgl_sprite_shader_render(struct cgl_sprite_shader *shader,
                              struct egl_mat4 *g_transform_mat,
                              struct egl_texture *texture, size_t sprite_count,
                              struct egl_sprite_buffer *sprite_buffer);

int cgl_tile_shader_load(struct cgl_tile_shader *out_shader,
                         struct em_arena *arena);

void cgl_tile_shader_destroy(struct cgl_tile_shader *shader);

void cgl_tile_shader_render(struct cgl_tile_shader *shader,
                            struct egl_ivec2 *g_tile_size,
                            struct egl_mat4 *g_transform_mat,
                            struct egl_texture *texture, size_t sprite_count,
                            struct egl_tile_buffer *tile_buffer);

#endif // CGL_OPENGL_H
