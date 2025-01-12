#pragma once

#include <engine/ea_allocator.h>
#include <engine/em_math.h>
#include <engine/eo_opengl.h>

#define COO_VERTEX_PER_QUAD 6

struct coo_debug_shader {
    GLuint program_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

struct coo_debug_vertex {
    struct em_ipos position;
    struct em_color color;
};

struct coo_debug {
    struct coo_debug_vertex vertex[COO_VERTEX_PER_QUAD];
};

struct coo_sprite_shader {
    GLuint program_id;
    GLint g_transform_mat_id;
#ifdef SHARED
    long long timestamp;
#endif // SHARED
};

struct coo_sprite_vertex {
    struct em_ipos position;
    struct em_itexpos texture;
};

struct coo_sprite {
    struct coo_sprite_vertex vertex[COO_VERTEX_PER_QUAD];
};

int coo_sprite_shader_load(struct coo_sprite_shader *out_shader,
                           struct ea_arena *arena);

void coo_sprite_shader_destroy(struct coo_sprite_shader *shader);

void coo_sprite_shader_render(struct coo_sprite_shader *shader,
                              struct em_mat4 *g_transform_mat,
                              struct eo_texture *texture, int count,
                              struct eo_buffer *sprite_buffer);

void coo_sprite_buffer_create(struct eo_buffer *out_buffer, int sprite_count);
#define coo_sprite_buffer_destroy eo_buffer_destroy

/*int coo_debug_shader_load(struct coo_debug_shader *out_shader,*/
/*                          struct ea_arena *arena);*/
/**/
/*void coo_debug_shader_destroy(struct coo_debug_shader *shader);*/
/**/
/*void coo_debug_shader_render(struct coo_debug_shader *shader,*/
/*                             struct em_mat4 *g_transform_mat, int count,*/
/*                             struct eo_buffer *debug_buffer);*/
/**/
/*void coo_debug_buffer_create(struct eo_buffer *out_buffer, int debug_count);*/
/*#define coo_debug_buffer_destroy eo_buffer_destroy*/
