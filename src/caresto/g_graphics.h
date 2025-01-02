#ifndef _G_OPENGL
#define _G_OPENGL

#include <GL/glew.h>
#include <caresto/mm_memory_management.h>

struct g_program {
    GLuint program_id;
};

int g_program_create(struct mm_arena *arena, struct g_program *out_program);

#endif
