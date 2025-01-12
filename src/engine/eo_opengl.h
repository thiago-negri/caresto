#pragma once

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <engine/ea_allocator.h>
#include <engine/em_math.h>
#include <stdint.h>

#define EO_SPRITE_MIRROR_X 1

struct eo_sprite {
    struct em_ipos position;
    struct em_isize size;
    struct em_itexpos texture_offset;
    unsigned int flags;
};

struct eo_sprite_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct eo_tile {
    // World position (top left)
    GLfloat x, y;

    // Texture position (top left)
    GLint u, v;
};

struct eo_tile_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct eo_texture {
    GLuint id;
#if SHARED
    long long timestamp;
#endif
};

struct eo_frame {
    double delta_time;
    SDL_Window *sdl_window;
};

int eo_shader_create(GLenum type, const GLchar *source, struct ea_arena *arena,
                     GLuint *out_shader_id);

int eo_program_link(GLuint program_id, size_t shader_count, GLuint *shaders,
                    struct ea_arena *arena);

void eo_sprite_buffer_create(GLsizei count,
                             struct eo_sprite_buffer *out_sprite_buffer);
void eo_sprite_buffer_destroy(struct eo_sprite_buffer *buffer);
void eo_sprite_buffer_data(struct eo_sprite_buffer *buffer, size_t count,
                           struct eo_sprite *data);

void eo_tile_buffer_create(GLsizei count,
                           struct eo_tile_buffer *out_tile_buffer);
void eo_tile_buffer_destroy(struct eo_tile_buffer *buffer);
void eo_tile_buffer_data(struct eo_tile_buffer *buffer, size_t count,
                         struct eo_tile *data);

int eo_texture_load(const char *file_path, struct eo_texture *out_texture);
void eo_texture_destroy(struct eo_texture *texture);

void eo_debug_message_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *user_param);
