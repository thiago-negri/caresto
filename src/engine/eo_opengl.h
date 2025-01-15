#pragma once

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <engine/ea_allocator.h>
#include <engine/em_math.h>
#include <stdint.h>

struct eo_texture {
    GLuint id;
#if SHARED
    long long timestamp;
#endif // SHARED
};

struct eo_frame {
    double delta_time;
    SDL_Window *sdl_window;
};

struct eo_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

int eo_shader_create(GLenum type, const GLchar *source, struct ea_arena *arena,
                     GLuint *out_shader_id);

int eo_program_link(GLuint program_id, size_t shader_count, GLuint *shaders,
                    struct ea_arena *arena);

void eo_buffer_create_start(struct eo_buffer *out_buffer, GLsizeiptr size,
                            GLenum usage);
void eo_buffer_create_end(void);
void eo_buffer_data(struct eo_buffer *buffer, GLsizeiptr size, void *data);
void eo_buffer_destroy(struct eo_buffer *buffer);

int eo_texture_load(const char *file_path, struct eo_texture *out_texture);
void eo_texture_destroy(struct eo_texture *texture);

void eo_debug_message_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *user_param);
