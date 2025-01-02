#ifndef G_OPENGL_H
#define G_OPENGL_H

#include <GL/glew.h>
#include <caresto/mm_memory_management.h>

// Can draw up to SPRITES_MAX per frame
#define G_SPRITE_MAX 1024
#define G_SPRITE_SIZE (sizeof(struct g_sprite))

// VAO and VBO are not linked to the program, but right now we only use it here
struct g_program {
    GLuint program_id;
    GLuint vertex_array_id;
    GLuint buffer_id;
};

struct g_texture {
    GLuint id;
};

struct g_sprite {
    GLfloat x;
    GLfloat y;
    GLfloat w;
    GLfloat h;
};

int g_program_create(struct mm_arena *arena, struct g_program *out_program);
void g_program_destroy(struct g_program *program);
int g_texture_load(const char *file_path, struct g_texture *out_texture);

#endif // G_OPENGL_H
