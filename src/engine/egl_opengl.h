#ifndef EGL_OPENGL_H
#define EGL_OPENGL_H

#include <stdint.h>

#include <GL/glew.h>

#include <engine/em_memory.h>

struct egl_sprite {
    // World position (top left)
    GLfloat x, y;

    // Size
    GLint w, h;

    // Texture position (top left)
    GLint u, v;
};

struct egl_sprite_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct egl_texture {
    GLuint id;
#if SHARED
    long long timestamp;
#endif
};

struct egl_mat4 {
    union {
        GLfloat values[16];
        struct {
            GLfloat ax, bx, cx, dx;
            GLfloat ay, by, cy, dy;
            GLfloat az, bz, cz, dz;
            GLfloat aw, bw, cw, dw;
        };
    };
};

struct egl_frame {
    uint64_t delta_time;
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

int egl_texture_load(const char *file_path, struct egl_texture *out_texture);
void egl_texture_destroy(struct egl_texture *texture);

void egl_ortho(struct egl_mat4 *out, GLfloat left, GLfloat right, GLfloat top,
               GLfloat bottom, GLfloat near, GLfloat far);
void egl_identity(struct egl_mat4 *out);

void egl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *user_param);

#endif // EGL_OPENGL_H
