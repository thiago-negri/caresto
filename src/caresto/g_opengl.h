#ifndef _G_OPENGL
#define _G_OPENGL

#include <GL/glew.h>
#include <caresto/mm_memory_management.h>

int g_opengl_program_create(struct mm_arena *arena, GLuint *out_program_id);

#endif
