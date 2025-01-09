#ifndef EGL_OPENGL_H
#define EGL_OPENGL_H

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <engine/em_memory.h>
#include <engine/eu_utils.h>
#include <stdint.h>

#define EGL_SPRITE_MIRROR_X 1

struct egl_sprite {
    struct eu_ipos position;
    struct eu_isize size;
    struct eu_itexpos texture_offset;
    unsigned int flags;
};

struct egl_sprite_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct egl_tile {
    // World position (top left)
    GLfloat x, y;

    // Texture position (top left)
    GLint u, v;
};

struct egl_tile_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct egl_texture {
    GLuint id;
#if SHARED
    long long timestamp;
#endif
};

struct egl_frame {
    uint64_t delta_time;
    SDL_Window *sdl_window;
};

int egl_shader_create(GLenum type, const GLchar *source, struct em_arena *arena,
                      GLuint *out_shader_id);

int egl_program_link(GLuint program_id, size_t shader_count, GLuint *shaders,
                     struct em_arena *arena);

void egl_sprite_buffer_create(GLsizei count,
                              struct egl_sprite_buffer *out_sprite_buffer);
void egl_sprite_buffer_destroy(struct egl_sprite_buffer *buffer);
void egl_sprite_buffer_data(struct egl_sprite_buffer *buffer, size_t count,
                            struct egl_sprite *data);

void egl_tile_buffer_create(GLsizei count,
                            struct egl_tile_buffer *out_tile_buffer);
void egl_tile_buffer_destroy(struct egl_tile_buffer *buffer);
void egl_tile_buffer_data(struct egl_tile_buffer *buffer, size_t count,
                          struct egl_tile *data);

int egl_texture_load(const char *file_path, struct egl_texture *out_texture);
void egl_texture_destroy(struct egl_texture *texture);

void egl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *user_param);

#endif // EGL_OPENGL_H
