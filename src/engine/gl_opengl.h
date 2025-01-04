#ifndef GL_OPENGL_H
#define GL_OPENGL_H

#include <GL/glew.h>

#include <engine/mm_memory_management.h>

// VAO and VBO are not linked to the program, but right now we only use it here
struct gl_program {
    GLuint program_id;
    GLint g_transform_mat_id;
};

struct gl_sprite {
    // World position (top left)
    GLfloat x, y;

    // Size
    GLint w, h;

    // Texture position (top left)
    GLint u, v;
};

struct gl_sprite_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct gl_texture {
    GLuint id;
};

struct gl_mat4 {
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

int gl_program_create(struct mm_arena *arena, struct gl_program *out_program);
void gl_program_destroy(struct gl_program *program);
void gl_program_render(struct gl_program *program,
                       struct gl_mat4 *g_transform_mat,
                       struct gl_texture *texture, size_t sprite_count,
                       struct gl_sprite_buffer *sprite_buffer);

void gl_sprite_buffer_create(GLsizei count,
                             struct gl_sprite_buffer *out_sprite_buffer);
void gl_sprite_buffer_destroy(struct gl_sprite_buffer *buffer);
void gl_sprite_buffer_data(struct gl_sprite_buffer *buffer, size_t count,
                           struct gl_sprite *data);

int gl_texture_load(const char *file_path, struct gl_texture *out_texture);
void gl_texture_destroy(struct gl_texture *texture);

void gl_ortho(struct gl_mat4 *out, GLfloat left, GLfloat right, GLfloat top,
              GLfloat bottom, GLfloat near, GLfloat far);
void gl_identity(struct gl_mat4 *out);

void gl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *user_param);

#endif // GL_OPENGL_H
