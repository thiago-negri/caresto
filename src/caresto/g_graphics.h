#ifndef G_OPENGL_H
#define G_OPENGL_H

#include <GL/glew.h>
#include <caresto/mm_memory_management.h>

// VAO and VBO are not linked to the program, but right now we only use it here
struct g_program {
    GLuint program_id;
    GLint g_transform_mat_id;
};

struct g_sprite {
    // World position (top left)
    GLfloat x, y;

    // Size
    GLint w, h;

    // Texture position (top left)
    GLint u, v;
};

struct g_sprite_buffer {
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct g_texture {
    GLuint id;
};

struct g_mat4 {
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

int g_program_create(struct mm_arena *arena, struct g_program *out_program);
void g_program_destroy(struct g_program *program);
void g_program_render(struct g_program *program, struct g_mat4 *g_transform_mat,
                      struct g_texture *texture, size_t sprite_count,
                      struct g_sprite_buffer *sprite_buffer);

void g_sprite_buffer_create(GLsizei count,
                            struct g_sprite_buffer *out_sprite_buffer);
void g_sprite_buffer_destroy(struct g_sprite_buffer *buffer);
void g_sprite_buffer_data(struct g_sprite_buffer *buffer, size_t count,
                          struct g_sprite *data);

int g_texture_load(const char *file_path, struct g_texture *out_texture);
void g_texture_destroy(struct g_texture *texture);

void g_ortho(struct g_mat4 *out, GLfloat left, GLfloat right, GLfloat top,
             GLfloat bottom, GLfloat near, GLfloat far);
void g_identity(struct g_mat4 *out);

void g_debug_message_callback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar *message, const void *user_param);

#endif // G_OPENGL_H
